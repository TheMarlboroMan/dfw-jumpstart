#include "audio_tools.h"

using namespace app;

int app::solve_line_for_x(tpoint p1, tpoint p2, tpos x)
{
	//TODO: This, of course, fails for straight or vertical lines.
	double m=(p2.y-p1.y) / (p2.x-p1.x); 	//m=(y2-y1) / (x2-x1)
	double b=p1.y-(m*p1.x); 		//y=mx+b, thus b=y-mx
	return (m*x) + b; 			//y=mx+b.
}


tools::ranged_value<int> app::calculate_volume(int volume, double max_radius, double min_radius, tpoint pt1, tpoint pt2)
{
	return {0, volume, solve_line_for_x(
		{max_radius, 0.}, 
		{min_radius, (double)volume}, 
		pt1.distance_to(pt2))};
}

lda::sound_panning app::calculate_panning(const ldv::rect& box, tpoint p)
{
	return lda::sound_panning::from_left(
		tools::ranged_value<int>{0, 255, solve_line_for_x(
			{(double)box.origin.x, 255.}, 
			{(double)(box.origin.x+box.w), 0.}, 
			(double)p.x)});
}

////////////////////////////

music_fader::music_fader(lda::audio_controller& a, const lda::resource_manager& r)
	:ac(a), rm(r), state(tstates::silence), curr_music(0), next_music(0),
	original_volume(ac.get_music_volume())
{

}

void music_fader::loop(float delta)
{
	switch(state)
	{
		case tstates::silence: 
		case tstates::playing:
		break;
		case tstates::fading:
			volume.tic(delta);
			ac.set_music_volume(volume());
			if(!volume())
			{
				if(next_music) begin_play();
				else stop_play();
			}
		break;
	}
}

void music_fader::play_music(size_t mus, unsigned int ms_fade)
{
	if(!mus) throw std::runtime_error("music_fader::play_music mus cannot be zero");
	//Check that music is not the same...
	if(mus == curr_music) return;
	
	next_music=mus;

	switch(state)
	{
		case tstates::silence:
			begin_play();
		break;
		case tstates::playing:
			if(!ms_fade) begin_play();
			else begin_fade(ms_fade);
		break;
		case tstates::fading:
			if(!ms_fade) begin_play();
		break;
	}
}

void music_fader::stop_music(unsigned int ms_fade)
{
	next_music=0;

	switch(state)
	{
		case tstates::silence: /*Do nothing.*/ break;
		case tstates::playing:
			if(ms_fade) begin_fade(ms_fade);
			else stop_play();
		break;
		case tstates::fading:
			if(!ms_fade) stop_play();
		break;
	}
}

void music_fader::begin_play()
{
	state=tstates::playing;
	ac.set_music_volume(original_volume);
	ac.play_music(rm.get_music(next_music));
	curr_music=next_music;
	next_music=0;
}

void music_fader::begin_fade(unsigned int ms)
{
	state=tstates::fading;
	volume=tools::linear_timed_function<float>::from_time(original_volume, 0, (float)ms/1000.f);
}

void music_fader::stop_play()
{
	state=tstates::silence;
	ac.stop_music();
	curr_music=0;
	next_music=0;
}
