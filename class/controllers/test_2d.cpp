#include "test_2d.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>
#include <templates/ranged_value.h>

//local
#include "../input.h"
#include "../app/game_input.h"
#include "../app/audio_defs.h"

using namespace app;

controller_test_2d::controller_test_2d(shared_resources& sr)
try
	:s_resources(sr),
	game_audio_dispatcher(sr.get_audio(), sr.get_audio_resource_manager()),
	game_camera{{0,0,700,500},{0,0}}, //This means that the camera always gets a 700x500 box, even with a larger window.
	game_localization(0, {"data/app_data/localization/descriptions"})
{
	game_localization.init();

	game_camera.set_center_margin({300, 200, 100, 100});
	game_camera.clear_limits();

	//TODO: I'd actually like to have this in the constructor.
	game_player.inject_dispatcher(game_audio_dispatcher);

	const auto& am=sr.get_arg_manager();

	//The thing starts at map01, terminus_id 0.
	if(am.value_exists_for("map") && am.value_exists_for("terminus"))
	{
		do_room_change(am.get_value("map"), std::atoi(am.get_value("terminus").c_str()));
	}
	else
	{
		do_room_change("map01.dat", 0);
	}
}
catch(std::exception& e)
{
	sr.get_log()<<"Unable to create main controller: "<<e.what()<<std::endl;
	//This would still propagate: initialization lists and exceptions work like that.
}

void controller_test_2d::loop(dfw::input& input, float delta)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	//World processing... First the audio players. So far roles are separate.
	const auto& cfb=game_camera.get_focus_box();
	app_interfaces::spatiable::t_box fb{(tpos)cfb.origin.x, (tpos)cfb.origin.y, (tdim)cfb.w, (tdim)cfb.h};
	for(auto& ap : game_room.get_audio_players()) ap.loop(delta, fb, game_player.get_poly().get_center());

	//Input processing and player logic.
	game_input gi;

	if(input.is_input_pressed(input_app::up)) gi.y=-1;
	else if(input.is_input_pressed(input_app::down)) gi.y=1;

	if(input.is_input_pressed(input_app::left)) gi.x=-1;
	else if(input.is_input_pressed(input_app::right)) gi.x=1;

	if(input.is_input_down(input_app::activate)) gi.activate=true;

	game_player.set_input(gi);
	game_player.step(delta, fb);

	//Player movement...
	auto movement_phase=[this, delta](motion::axis axis)
	{
		//This is the laziest approach: revert the movement as soon as 
		//a collision is detected, opting for an early exit.
		//Works with walls of different shapes but may leave the player
		//visibly separated from the wall. Of course, could be made complex so
		//axis aligned boxes are taken into account too.

		//Lambda within a lambda :D.
		auto solve_collisions=[this, axis](const std::vector<const app_interfaces::spatiable *>& collisions) 
		{
			if(!collisions.size()) return;

			for(const auto& c: collisions)
				if(game_player.is_colliding_with(*c))
				{
					game_player.cancel_movement(axis);
					break;
				}
		};

		game_player.integrate_motion(delta, axis);

		auto obstacle_collisions=game_room.get_obstacles();
		solve_collisions(obstacle_collisions);

		//This should not be necessary, actually, as any collision with an object disqualifies collisions with walls but well...
		auto coarse_collisions=game_room.get_walls_by_box(app_interfaces::coarse_bounding_box(game_player));
		solve_collisions(coarse_collisions);

		//Another option is more complex and cumbersome: a binary search
		//is conducted to the nearest free position in N iterations
		//is found. All iterations are checked against still colliding
		//walls. Different shapes work too.
	};

	if(gi.x) movement_phase(motion::axis::x);
	if(gi.y) movement_phase(motion::axis::y);

	//Now effect collisions... These only apply to the final position, by design. Not like level design allows crazy things...
	if(gi.x || gi.y)
	{
		//Check if there was trigger memory and if we need to clear it.
		auto ttm=game_room.get_trigger_memory();
		if(ttm && !game_player.is_colliding_with(*ttm)) game_room.clear_trigger_memory();
		
		//Now se if we are colliding with any trigger...ll 
		const auto& trig=game_room.get_triggers();
		auto it_touch=std::find_if(std::begin(trig), std::end(trig), [this, ttm](const object_trigger& tr)
		{
			return tr.is_touch() 				//Is touch trigger...
				&& game_player.is_colliding_with(tr)	//The player is touching
				&& (!ttm || !(*ttm == tr));		//If there's a memory trigger, it is not this one.
		});

		if(it_touch!=std::end(trig)) 
		{
			do_trigger(*it_touch);
			return;
		}
	}

	if(gi.activate)
	{
		const auto& act_poly=game_player.get_activate_poly();
		const auto& trig=game_room.get_triggers();
		auto it_act=std::find_if(std::begin(trig), std::end(trig), [this, &act_poly](const object_trigger& tr)
		{
			return tr.is_activate()				//Is activate trigger...
				&& tr.is_colliding_with(act_poly);	//The player activation box is touching
		});
		if(it_act!=std::end(trig)) do_trigger(*it_act);
	}
	
	game_camera.center_on(
		game_draw_struct.drawable_box_from_spatiable(
			game_player));
}

void controller_test_2d::draw(ldv::screen& screen, int fps)
{
	screen.clear(ldv::rgba8(0, 0, 0, 0));
	
#ifndef WDEBUG_CODE
	auto dc=game_room.get_drawables();

	//First layer...
	for(const auto& d: dc.background)
	{
		d->draw(screen, game_camera, game_draw_struct, s_resources);
	}

	//Second layer... add player and sort.
	dc.main.push_back(&game_player);
	std::sort(std::begin(dc.main), std::end(dc.main), app_interfaces::drawable_order); 

	for(const auto& d: dc.main)
	{
		d->draw(screen, game_camera, game_draw_struct, s_resources);
	}

	//Draw fps.
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 12), 
		ldv::rgba8(0, 0, 255, 255), compat::to_string(fps)};
	fps_text.go_to({500,0});
	fps_text.draw(screen);

#else

	if(s_resources.get_debug_config().bool_from_path("debug:video:center_camera"))
	{
		game_camera.center_on(
			game_draw_struct.drawable_box_from_box_polygon(game_player.get_poly()));
	}

	if(s_resources.get_debug_config().bool_from_path("debug:video:draw_background"))
	{
		for(const auto& d: game_room.get_drawables().background)
		{
			d->draw(screen, game_camera, game_draw_struct, s_resources);
		}
	}

	if(s_resources.get_debug_config().bool_from_path("debug:video:draw_main_plane"))
	{
		auto dc=game_room.get_drawables();
		dc.main.push_back(&game_player);
		std::sort(std::begin(dc.main), std::end(dc.main), app_interfaces::drawable_order); 
		for(const auto& d: dc.main)
		{
			d->draw(screen, game_camera, game_draw_struct, s_resources);
		}
	}

	if(s_resources.get_debug_config().bool_from_path("debug:video:draw_fps"))
	{
		ldv::ttf_representation fps_text{
			s_resources.get_ttf_manager().get("consola-mono", 12), 
			ldv::rgba8(0, 0, 255, 255), compat::to_string(fps)};
		fps_text.go_to({500,0});
		fps_text.draw(screen);
	}	

	auto draw_bounding_box=[this, &screen](const app_interfaces::spatiable& s, ldv::rgb_color col)
	{
		game_draw_struct.set_type(app::draw_struct::types::box);
		game_draw_struct.set_color(col);
		game_draw_struct.set_alpha(128);
		game_draw_struct.set_primitive_fill(ldv::polygon_representation::type::fill);
		game_draw_struct.set_box_location(game_draw_struct.drawable_box_from_spatiable(s));
		game_draw_struct.rep->draw(screen, game_camera);
	};

	if(s_resources.get_debug_config().bool_from_path("debug:video:draw_wall_bounding_boxes"))
	{
		//Draw collision boxes of all walls.
		const auto w=game_room.get_all_walls();
		auto fdraw_walls=[this, &screen, draw_bounding_box](const room_wall& wall)
		{
			draw_bounding_box(wall, ldv::rgb8(0,255,0));
		};
		w.apply(fdraw_walls);
	}

	if(s_resources.get_debug_config().bool_from_path("debug:video:draw_player_bounding_boxes"))
	{
		draw_bounding_box(game_player, ldv::rgb8(255,0,0));
	}

	//A set of dots...
	/*
	ldv::point_representation point_grid{ldv::rgb8(0,255,0)};
	for(int x=0; x < 700; x+=32)
		for(int y=0; y<500; y+=32)
			point_grid.insert({x, y});

	point_grid.draw(screen);
	*/
#endif
}

bool controller_test_2d::can_leave_state() const
{
	return true;
}

//TODO: This will dissapear.
void controller_test_2d::do_room_change(const std::string& map, int terminus_id)
{
	//TODO: Clear any lingering data belonging to this controller...

	//TODO: Stop sounds. I think that will happen when the map loads, but just in case.
	
	game_room.load(map);
	game_room.inject_audio_dispatcher(game_audio_dispatcher);
	game_player.center_on(game_room.get_entrance_by_id(terminus_id));

	game_camera.set_limits({0,0, game_room.get_w(), game_room.get_h()});
	game_camera.center_on(
		game_draw_struct.drawable_box_from_spatiable(
			game_player));

	//TODO: Set player bearing upon entrance.
}

void controller_test_2d::do_room_change(const room_action_exit& a)
{
	do_room_change(a.room, a.terminus_id);
}

void controller_test_2d::do_text_display(const room_action_text& a)
{
	//TODO: Perhaps stop sounds, right? Or maybe we just leave them. Try it.
	std::string dnotstr("data:{txt: \""+game_localization.get(a.text_id)+"\"}");
	auto tok=tools::dnot_parse_string(dnotstr);
	broadcast({0, tok}); //TODO: This may just be the message type, the 0.
	set_state(state_test_2d_text);
}

void controller_test_2d::do_console_transition(const room_action_console&)
{

}

void controller_test_2d::do_arcade_transition(const room_action_arcade&)
{

}

void controller_test_2d::do_trigger(const object_trigger& trig)
{
	if(trig.is_touch())
		game_room.set_trigger_memory(trig);
 
	const room_action * act=game_room.get_action(trig.get_action_id());
	if(!act)
	{
		s_resources.get_log()<<"unavailable action "<<trig.get_action_id()<<std::endl;
		return;
	}
	
	//If it is repeatable we may skip the action.
	if(!act->repeat)
	{
		if(unique_actions.count(act->action_id)) return;
		s_resources.get_log()<<"Adding unique trigger "<<act->action_id<<std::endl;
		unique_actions.insert(act->action_id);
	}

	//Pure win.
	struct d:
		public action_dispatcher
	{
					d(controller_test_2d * ct):c{ct}{}

		virtual void		resolve(const room_action_exit& a) 	{c->do_room_change(a);}
		virtual void		resolve(const room_action_text& a) 	{c->do_text_display(a);}
		virtual void		resolve(const room_action_console& a) {c->do_console_transition(a);}
		virtual void		resolve(const room_action_arcade& a) 	{c->do_arcade_transition(a);}
		controller_test_2d *	c;
	}ad(this);
	act->dispatch(ad);
}
