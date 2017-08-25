#include "test_2d_text.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test_2d_text::controller_test_2d_text(shared_resources& sr)
	:s_resources(sr)
{

}

void controller_test_2d_text::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		//TODO: Do this good.
		set_state(state_test_2d);
//		set_leave(true);
		return;
	}
}

void controller_test_2d_text::draw(ldv::screen& screen, int /*fps*/)
{
	ldv::box_representation box{
		ldv::polygon_representation::type::fill, 
		{0,0,screen.get_w(), screen.get_h()}, 
		ldv::rgba8(0,0,128,128)
	};
	box.set_blend(ldv::representation::blends::alpha);

	box.draw(screen);
}

void controller_test_2d_text::request_draw(dfw::controller_view_manager& cvm)
{
	cvm.add(state_test_2d); 
	cvm.add_ptr(this);
}
