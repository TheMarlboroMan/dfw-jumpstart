#include "test.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test::controller_test(ldt::log& plog, ldv::resource_manager& /*vm*/, const tools::ttf_manager& pttf_man)
	:log(plog), ttf_man(pttf_man)

{

}

void controller_test::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

//	if(input.is_input_down(input_app::pause)) paused=!paused;

	
}

void controller_test::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 255, 255, 255));
	
	ldv::box_representation box1(ldv::polygon_representation::type::line, ldv::rect{10,10,40,40}, ldv::rgba_color{255, 0, 0, 255});
	box1.draw(screen);

	ldv::box_representation box2(ldv::polygon_representation::type::fill, ldv::rect{40,40,40,40}, ldv::rgba8(0, 0, 0, 16));
	box2.set_blend(ldv::representation::blends::alpha);
	box2.draw(screen);
}

void controller_test::awake(dfw::input& /*input*/)
{

}

void controller_test::slumber(dfw::input& /*input*/)
{

}

bool controller_test::can_leave_state() const
{
	return true;
}
