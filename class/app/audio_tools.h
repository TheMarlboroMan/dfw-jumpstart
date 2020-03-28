#pragma once

//local
#include "spatiable.h"
#include "app_defs.h"

//tools
#include <tools/ranged_value.h>
#include <tools/linear_timed_function.h>

//libdan2
#include <lda/audio_controller.h>
#include <lda/resource_manager.h>
#include <lda/audio_callback_interface.h>
#include <lda/sound.h>

namespace app
{

//This is to be used by composition if any aurable needs callbacks,
//assignable through aurable_callback.fnc=[=](){my_method_here();} using 
//lambdas with full capture.
struct aurable_callback:
	public lda::audio_callback_interface
{
	virtual void			do_callback() {fn();}
	std::function<void(void)>	fn;
};

int				solve_line_for_x(tpoint, tpoint, tpos);
tools::ranged_value<int>	calculate_volume(int, double, double, tpoint, tpoint);
lda::sound_panning		calculate_panning(const ldv::rect&, tpoint);

//As much as SDL_Mixer has fade functions it is always fun to roll your own.

class music_fader
{
	public:

				music_fader(lda::audio_controller&, const lda::resource_manager&);
	void			loop(float);
	void			play_music(size_t, unsigned int, unsigned int, unsigned int);
	void			stop_music(unsigned int);

	private:

	void			begin_play(int);
	void			begin_fadein(unsigned int);
	void			begin_fadeout(unsigned int);
	void			stop_play();

	lda::audio_controller&			ac;
	const lda::resource_manager&		rm;
	enum class tstates{silence, playing, fading_out, fading_in} state;
	size_t					curr_music,
						next_music;
	tools::linear_timed_function<float>	fadeout,
						fadein;
	int					target_volume,
						next_fadeout,
						next_fadein;
};

}
