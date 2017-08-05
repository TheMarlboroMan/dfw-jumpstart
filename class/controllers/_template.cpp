#include "_template.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_template::controller_template(ldt::log& plog)
	:log(plog)
{

}

void controller_template::preloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_template::loop(dfw::input& /*input*/, float /*delta*/)
{
	/*
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}
	*/
}

void controller_template::postloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_template::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 255, 255, 255));
}

void controller_template::awake(dfw::input& /*input*/)
{

}

void controller_template::slumber(dfw::input& /*input*/)
{

}

bool controller_template::can_leave_state() const
{
	return true;
}
