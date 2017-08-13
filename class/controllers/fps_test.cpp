#include "fps_test.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_fps_test::controller_fps_test(shared_resources& sr)
	:s_resources(sr), frames(0), frames_accum(0), frames_measured(0),
	seconds(0)
{

}

void controller_fps_test::preloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_fps_test::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}
	
}

void controller_fps_test::postloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_fps_test::draw(ldv::screen& screen, int fps)
{
	int csec=s_resources.get_controller_chrono().get_seconds(),
		cmsec=s_resources.get_controller_chrono().get_milliseconds();

	++frames;

	if(seconds!=csec)
	{
		screen.clear(ldv::rgba8(0, 255, 255, 255));
		seconds=csec;
		frames_measured=frames;
		frames_accum+=frames;
		frames=0;
	}
	else
	{
		screen.clear(ldv::rgba8(0, 0, 0, 0));
	}

	std::string fdata="sec: "+compat::to_string(csec)+
	" msec: "+compat::to_string(cmsec)+
	" frames:"+compat::to_string(frames_accum)+
	" fps"+compat::to_string(fps)+"/"+compat::to_string(frames_measured);

	//Draw fps.
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(0, 255, 255, 255), fdata};
	fps_text.go_to({0,0});
	fps_text.draw(screen);
}

void controller_fps_test::awake(dfw::input& /*input*/)
{

}

void controller_fps_test::slumber(dfw::input& /*input*/)
{

}

bool controller_fps_test::can_leave_state() const
{
	return true;
}
