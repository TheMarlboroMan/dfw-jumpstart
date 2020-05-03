#include "../../include/app/audio_tools.h"

using namespace app;

music_fader::music_fader(lda::audio_controller& a, const lda::resource_manager& r)
	:ac(a), rm(r), state(tstates::silence), curr_music(0), next_music(0),
	target_volume(0), next_fadeout(0), next_fadein(0)
{

}

/** Will get music, ms of fadein and out and a volume value.

The rules are:

From silence: if no fadein is given, the music will play right away. Else it will fade in.
From playing: if a fadeout is given, it will begin fading. If a fadein is given, 
	it will happen after the fadeout or right away (if no fadeout). It no
	fadeout or fadein are given, the music will play right away.
From fading in or out: the current fade must end.
*/

void music_fader::play_music(size_t mus, unsigned int ms_fadeout, unsigned int ms_fadein, unsigned int vol)
{
	if(!mus) throw std::runtime_error("music_fader::play_music mus cannot be zero");
	//Check that music is not the same...
	if(mus == curr_music) return;

	next_music=mus;
	target_volume=vol;

	switch(state)
	{
		case tstates::fading_in:
		case tstates::fading_out: //Nothing here, just wait until the current fade is over.
		break;
		case tstates::silence:
			if(!ms_fadein) begin_play(target_volume);
			else begin_fadein(ms_fadein);
		break;
		case tstates::playing:

			if(!ms_fadein && !ms_fadeout) begin_play(target_volume);
			else
			{
				next_fadein=ms_fadein;
				next_fadeout=ms_fadeout;

				if(ms_fadeout) begin_fadeout(ms_fadeout);
				else begin_fadein(ms_fadein);
			}
		break;
	}
}

/*
Stops music, either dead on its tracks or with a fadeout from playing or any
fade effect.
*/

void music_fader::stop_music(unsigned int ms_fade)
{
	next_music=0;

	switch(state)
	{
		case tstates::silence: /*Do nothing.*/ break;
		case tstates::fading_in:
		case tstates::playing:
		case tstates::fading_out:
			if(ms_fade) begin_fadeout(ms_fade);
			else 
			{
				stop_play();
				fadein.reset(0,0,0);
				fadeout.reset(0,0,0);
			}
		break;
	}
}

void music_fader::loop(float delta)
{
	switch(state)
	{
		case tstates::silence: 
		case tstates::playing:
			//Do nothing...
		break;
		case tstates::fading_in:
			fadein.tic(delta);
			ac.set_music_volume(fadein);
			if(fadein.is_done()) 
			{
				next_fadein=0;
				fadein.reset(0,0,0);
				if(next_fadeout) 
				{
					begin_fadeout(next_fadeout);
					next_fadeout=0;
				}
				else state=tstates::playing;
			}
		break;
		case tstates::fading_out:
			fadeout.tic(delta);
			ac.set_music_volume(fadeout);
			if(fadeout.is_done())
			{
				next_fadeout=0;
				fadeout.reset(0,0,0);
				if(!next_music) stop_play();
				else
				{
					if(next_fadein) 
					{
						begin_fadein(next_fadein);
						next_fadein=0;
					}
					else begin_play(target_volume);
				}
			}
		break;
	}
}

void music_fader::begin_play(int v)
{
	ac.set_music_volume(v);
	ac.play_music(lda::music_struct{rm.get_music(next_music), -1, -1});
	curr_music=next_music;
	next_music=0;
	state=tstates::playing;
}

void music_fader::begin_fadein(unsigned int tfade)
{
	begin_play(0);
	fadein=tools::linear_timed_function<float>::from_time(0., target_volume, (float)tfade/1000.f);
	state=tstates::fading_in;
}

void music_fader::begin_fadeout(unsigned int tfade)
{
	fadeout=tools::linear_timed_function<float>::from_time(ac.get_music_volume(), 0, (float)tfade/1000.f);
	state=tstates::fading_out;
}

void music_fader::stop_play()
{
	state=tstates::silence;
	ac.stop_music();
	curr_music=0;
	next_music=0;
	next_fadein=0;
	next_fadeout=0;
}

//////////////////////////////////////////


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
