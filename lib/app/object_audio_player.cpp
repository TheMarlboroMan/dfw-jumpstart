#include "../../include/app/object_audio_player.h"

//tools
#include <tools/number_generator.h>
#include <tools/ranged_value.h>

using namespace app;

object_audio_player::object_audio_player(tpoint pt, ttype t, object_audio_player_data d)
	:room_object_box({pt.x, pt.y, 1, 1}), type(t), data(d)
{
	calculate_time();
}

object_audio_player::~object_audio_player()
{
	if(channel.is_linked())
	{
		//The order is essential: stop will invoke the callback if still available. The callback DOES unlink, causing crashes when doing further actions on the channel.
		channel.set_monitoring(false);
		channel.clear_callback_listener();
		if(channel.is_playing()) channel.stop();
	}
}

void object_audio_player::on_audio_stop()
{
	//TODO: I think that with "unmonitor" and "unlink" I win: if unmonitored, the channel will do "free" and that will clear all.
	channel.set_monitoring(false);
	channel.clear_callback_listener();
	channel.unlink();

	if(data.replays)
	{
		--data.replays;
		calculate_time();
	}
}

//The whole thing is done without states. Arguably, it would be MUCH easier to
//use states for this, right?. Just "waiting", "acquiring" and "playing".

void object_audio_player::loop(float delta)
{
	//Am I not playing? To be playing one's gotta have a channel and that channel must be playing.
	if(! (channel.is_linked() && channel.is_playing()))
	{
		if(can_play())
		{
			if(acquire_channel())
			{
				play_audio();
			} //If I could not, channel acquisition failed: no more channels available...
		}
		else	//Do my waiting...
		{
			data.remaning_time-=delta;
			if(data.remaning_time < 0.f) data.remaning_time=0.f;
		}
	}
	else
	{
		do_play_logic();
	}
}

void object_audio_player::do_play_logic()
{
	switch(type)
	{
		case ttype::ambient:
			channel.clear_panning();
		break;
		case ttype::source:
		{
			auto center=get_poly().get_centroid();
			channel.set_stereo_volume(calculate_panning(dispatcher->request_view_rect(), center));
			channel.set_volume(calculate_volume(data.volume, data.max_radius, data.min_radius, center, dispatcher->request_player_pos()));
		}
		break;
	}
}

bool object_audio_player::acquire_channel()
{
	try
	{
		channel=dispatcher->request_audio_channel();
		channel.set_monitoring(true); //Funny, you can actually NOT do this and the result would be the same in this case... but I want to try it.
		play_stops_cb.fn=[=](){on_audio_stop();};
		channel.assign_callback_listener(play_stops_cb);
		return true;
	}
	catch(std::exception& e) {}

	return false;
}

bool object_audio_player::can_play() const
{
	return !data.remaning_time;
}

//Almost better to subclass, actually.
void object_audio_player::play_audio()
{
	lda::sound_panning panning;
	tools::ranged_value<int> volume{0, data.volume, data.volume};

	switch(type)
	{
		case ttype::ambient:
		break;
		case ttype::source:
		{
			auto center=get_poly().get_centroid();
			panning=calculate_panning(dispatcher->request_view_rect(), center);
			volume=calculate_volume(data.volume, data.max_radius, data.min_radius, center, dispatcher->request_player_pos());
		}
		break;
	}

	channel.play({
		dispatcher->request_sound_resource(data.sound_id),
		volume, 
		data.replays==-1 && !data.min_ms_delay_repeat && !data.random_ms_delay_repeat ? -1 : 0, //repeats are actually code_driven... Only if it repeats non stop will we ask the mixer to repeat until the end of times.
		panning,
		0}); //No fading for you.
}

void object_audio_player::calculate_time()
{
	tools::int_generator g(0, data.random_ms_delay_repeat);
	data.remaning_time=((float)data.min_ms_delay_repeat/1000.f)+((float)g()/1000.f);
}
