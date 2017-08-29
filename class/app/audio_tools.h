#ifndef AUDIO_TOOLS_H
#define AUDIO_TOOLS_H

//libdan2
#include <audio/audio_callback_interface/audio_callback_interface.h>
#include <audio/sound/sound.h>

//tools
#include <templates/ranged_value.h>

//local
#include "spatiable.h"
#include "app_defs.h"

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

int				solve_line_for_x(app_interfaces::spatiable::t_point, app_interfaces::spatiable::t_point, tpos);
tools::ranged_value<int>	calculate_volume(int, double, double, app_interfaces::spatiable::t_point, app_interfaces::spatiable::t_point);
lda::sound_panning		calculate_panning(const app_interfaces::spatiable::t_box&, app_interfaces::spatiable::t_point);

}
#endif
