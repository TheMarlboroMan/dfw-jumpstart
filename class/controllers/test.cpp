#include "test.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"
#include "../app/game_input.h"

using namespace app;

controller_test::controller_test(shared_resources& sr)
try
	:s_resources(sr),
	game_camera{{0,0,700,500},{0,0}} //This means that the camera always gets a 700x500 box, even with a larger window.
{
	game_camera.set_center_margin({300, 200, 100, 100});

	//The thing starts at map01, terminus_id 0.
	do_room_change("map01.dat", 0);
}
catch(std::exception& e)
{
	sr.get_log()<<"Unable to create main controller: "<<e.what()<<std::endl;
	//This would still propagate: initialization lists and exceptions work like that.
}

void controller_test::loop(dfw::input& input, float delta)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	//Input processing.
	game_input gi;

	if(input.is_input_pressed(input_app::up)) gi.y=-1;
	else if(input.is_input_pressed(input_app::down)) gi.y=1;

	if(input.is_input_pressed(input_app::left)) gi.x=-1;
	else if(input.is_input_pressed(input_app::right)) gi.x=1;

	game_player.set_input(gi);
	game_player.step(delta);

	//Player movement...

	//TODO: This can be adjusted into a simple function.

//	auto movement_phase=[this](motion::axis axis, player::

	if(gi.x) //Phase x
	{
		game_player.integrate_motion(delta, motion::axis::x);
		auto collisions=game_room.get_walls_by_box(game_player.get_box());
		if(collisions.size())
		{

			game_player.adjust_collision_horizontal(*collisions[0]);
		}
	}

	if(gi.y) //Phase y
	{
		game_player.integrate_motion(delta, motion::axis::y);
		auto collisions=game_room.get_walls_by_box(game_player.get_box());
		if(collisions.size())
		{
			game_player.adjust_collision_vertical(*collisions[0]);
		}
	}

	if(gi.x || gi.y)
	{
		//Now effect collisions... These only apply to the final position, by design. Not like level design allows crazy things...
		struct {
			std::string 	map;
			int		terminus_id;
			bool 		change=false;
		}room_change;

		//Interestingly, we should not enforce "do_room_change" in here, since it will clear the room, hence the very
		//same thing we are iterating.
		for(const auto& e : game_room.get_exits())
		{
			if(game_player.is_colliding_with(e))
			{
				room_change.map=e.get_destination_map();
				room_change.terminus_id=e.get_terminus_id();
				room_change.change=true;
				break;
			}
		}

		if(room_change.change) 
		{
			do_room_change(room_change.map, room_change.terminus_id);
		}
	}
}

void controller_test::draw(ldv::screen& screen, int fps)
{
	screen.clear(ldv::rgba8(0, 0, 0, 0));
	
	//Have camera follow the player... This would actually belong in
	//the logic part, but it iches a bit. Still, why center if not moving?

	game_camera.center_on(
		game_draw_struct.draw_box_from_spatiable_box(
			game_player.get_box()));

	const auto& dc=game_room.get_drawables();

	//First layer...
	for(const auto& d: dc.background)
	{
		d->draw(screen, game_camera, game_draw_struct, s_resources);
	}

	//Second layer: game objects... this has to be ordered...
	//TODO: Get the vector from the room, add the player, order, draw.
	game_player.draw(screen, game_camera, game_draw_struct, s_resources);

	//Draw fps.
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 12), 
		ldv::rgba8(255, 255, 255, 255), compat::to_string(fps)};
	fps_text.go_to({500,0});
	fps_text.draw(screen);
}

bool controller_test::can_leave_state() const
{
	return true;
}

void controller_test::do_room_change(const std::string& map, int terminus_id)
{
	//TODO: Clear any lingering data belonging to this controller...
	
	game_room.load(map);
	game_player.center_on(game_room.get_entrance_by_id(terminus_id));

	ldv::rect room_box={0,0, game_room.get_w(), game_room.get_h()};
	game_camera.set_limits(room_box);
	game_camera.go_to({0,0});
	//TODO: Set player bearing upon entrance.
}
