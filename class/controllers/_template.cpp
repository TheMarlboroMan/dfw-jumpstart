#include "_template.h"

//local
#include "../input.h"

using namespace app;

controller_template::controller_template(ldt::log& plog)
	:log(plog)
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

void controller_template::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 255, 255, 255));
}
