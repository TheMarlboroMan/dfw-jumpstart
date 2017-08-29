#ifndef OBJECT_AUDIO_PLAYER_H
#define OBJECT_AUDIO_PLAYER_H

//local
#include "aurable.h"
#include "room_object_box.h"
#include "audio_tools.h"


namespace app
{

//Just in case we need more parameters, to avoid making a constructor mess...
struct object_audio_player_data
{
	int			sound_id,
				volume,
				replays, //-1 means infinite...
				min_ms_delay_repeat,
				random_ms_delay_repeat;

	float			remaning_time;

				//Source type specifics...
	double			max_radius,	//For source, beyond this, volume is 0.
				min_radius;	//For source, beyond this, volume is "volume".

	
};


//A link of logic object that plays sounds. Can either play on two channels
//or in a directional way. Needs to be injected with the channel_dispatcher_interface
//to request its channel, of course.

class object_audio_player:
	public app_interfaces::aurable,
	public room_object_box
{
	public:

	//TODO: Almost better to have two of these.
	enum class 			ttype{ambient, source};

					object_audio_player(app_interfaces::spatiable::t_point, ttype, object_audio_player_data);
					//	room_object_box(const app_interfaces::spatiable::t_box& b)
					~object_audio_player(); //Needs to make the sound stop and FREE the channel.

					//The box is the camera, the point is the player.
	void				loop(float, const app_interfaces::spatiable::t_box&, app_interfaces::spatiable::t_point);

	private:

	void				do_play_logic(const app_interfaces::spatiable::t_box&, app_interfaces::spatiable::t_point);
	void				play_audio(const app_interfaces::spatiable::t_box&, app_interfaces::spatiable::t_point);
	void				on_audio_stop();
	bool				acquire_channel();
	bool				can_play() const;
	void				calculate_time();

	ttype				type;
	object_audio_player_data	data;

					//Audio specifics.
	lda::audio_channel			channel;
	aurable_callback			play_stops_cb;	//So the channel can be freed.
};

}

#endif
