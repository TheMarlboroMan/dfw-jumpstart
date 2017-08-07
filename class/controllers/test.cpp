#include "test.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test::controller_test(shared_resources& sr)
try
	:s_resources(sr),
	game_camera{{0,0,700,500},{0,0}} //This means that the camera always gets a 700x500 box, even with a larger window.
{
	game_room.load("map01.dat");
}
catch(std::exception& e)
{
	sr.get_log()<<"Unable to create main controller: "<<e.what()<<std::endl;
	//This would still propagate: initialization lists and exceptions work like that.
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
	screen.clear(ldv::rgba8(0, 0, 0, 0));
	
	const auto& dc=game_room.get_drawables();

	//First layer...
	for(const auto& d: dc.background)
	{
		d->draw(screen, game_camera, game_draw_struct, s_resources);
	}

	//Second layer: game objects... this has to be ordered...
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
