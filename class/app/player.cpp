#include "player.h"

//tools
#include <class/number_generator.h>

//local
#include "display_defs.h"
#include "audio_defs.h"
#include "audio_tools.h"

using namespace app;

player::player():
	polygon{ {{0.,0.}, {w,0.}, {w, h}, {0., h}}, {w/2, h/2}},
	prev_polygon(polygon),
	player_bearing{2},
	walk_distance{0.f}, walk_time{0.f},
	step_sounds{sound_defs::step_01, sound_defs::step_02},
	step_sounds_index{0} {

}

void player::step(float _delta) {

	if(motion_data.has_motion()) {

		walk_time+=_delta;

		//In an ideal world, the concern of sound is separated. Not here.
		//Anyway, this controls the distance walked consecutively, 
		//which will determine when a step sound is played.

		if(walk_distance >= walk_step_distance) {

			walk_distance=0.f;

			try {
				//Fire and forget.
				tools::int_generator v(30,40); //Random volume...
				size_t sndindex=step_sounds_index % step_sounds.size();
				auto channel=dispatcher->request_audio_channel();
				channel.play({
					dispatcher->request_sound_resource(step_sounds[sndindex]),
					v(), 0, //Volume and repeats.
					calculate_panning(dispatcher->request_view_rect(), polygon.get_centroid()), 0}); //Panning and fade.

				++step_sounds_index;
			}
			catch(std::exception &e) {
			/*Noop*/}
		}
	}
}

bool player::is_in_camera(const ldv::rect&) const {
	//TODO.
	return false;
}

void player::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& sr) const {
	//Now the animation...
	const auto& frame=sr.get_animation(animation_defs::player).get(choose_animation_frame()).get_for_time(walk_time).frame;
	const auto& frect=frame.get_rect();

	ds.set_type(app::draw_struct::types::bitmap);
	ds.set_texture(sr.get_video_resource_manager().get_texture(texture_defs::player_sheet));
	ds.set_clip(frect);
	ds.set_location(
		ds.location_projection(
			ds.drawable_box_from_box_polygon(polygon), 
			frect, frame.disp_x, frame.disp_y));
	ds.rep->draw(scr, cam);
}

//Receive game input and alter its internal representation as a consequence.

void player::set_input(game_input gi) {

	if(!gi.x && !gi.y) {
		motion_data.set_vector({0.0, 0.0});
	}
	else {
		motion_data.set_vector(gi.x, motion::axis::x);
		motion_data.set_vector(gi.y, motion::axis::y);

		//Actually, this is an overloaded operator.
		player_bearing=motion_data.get_vector();
	}
}

void player::cancel_movement(motion::axis axis) {

	polygon=prev_polygon;
	motion_data.set_vector(0.0, axis);
}

void player::confirm_movement_stage() {

	prev_polygon=polygon;
}

void player::start_movement_phase() {

	prev_polygon=polygon;
	prev_position=get_poly().get_vertex(0);
}

void player::solve_movement_phase() {

	if(!motion_data.has_motion()) {

		walk_time=0.f;
		walk_distance=0.f;
	}
	else {
		auto pos=get_poly().get_vertex(0);

		if(pos!=prev_position) {
			walk_distance+=abs(ldt::distance_between(prev_position, pos));
		}
	}
}

void player::integrate_motion(float delta, motion::axis axis) {

	float v=motion_data.get_vector(axis);
	float nv=(v*delta)*speed;

	switch(axis) {
		case motion::axis::x:
			move_by({nv, 0.}); break;
		case motion::axis::y:
			move_by({0., nv}); break;
	}
}

int player::choose_animation_frame() const {

	switch(player_bearing()) {
		case bearing::tbearing::n:
			if(motion_data.has_motion()) 	return player_animation_defs::walk_n;
			else 				return player_animation_defs::idle_n;
		break;
		case bearing::tbearing::ne:
		case bearing::tbearing::e:
		case bearing::tbearing::se:
			if(motion_data.has_motion()) 	return player_animation_defs::walk_e;
			else 				return player_animation_defs::idle_e;
		break;
		case bearing::tbearing::s:
			if(motion_data.has_motion()) 	return player_animation_defs::walk_s;
			else 				return player_animation_defs::idle_s;
		break;
		case bearing::tbearing::sw:
		case bearing::tbearing::w:
		case bearing::tbearing::nw:
			if(motion_data.has_motion()) 	return player_animation_defs::walk_w;
			else 				return player_animation_defs::idle_w;
		break;
	}

	///Stupid compiler...
	return player_animation_defs::idle_n;
}

tpoly player::get_activate_poly() const {

	const auto& origin=polygon.get_vertex(0);
	tpos	x=origin.x, y=origin.y;
	
	//I am sure there's a more satisfying solution, but well...

	switch(player_bearing()) {
		case bearing::tbearing::n: 
			y-=h;
		break;
		case bearing::tbearing::ne:
			x+=w; 		y-=h;
		break;
		case bearing::tbearing::e:
			x+=w;
		break;
		case bearing::tbearing::se:
			x+=w; y+=h;
		break;
		case bearing::tbearing::s:
			y+=h;
		break;
		case bearing::tbearing::sw:
			x-=w; y+=h;
		break;
		case bearing::tbearing::w:
			x-=w;
		break;
		case bearing::tbearing::nw:
			x-=w; y-=h;
		break;
	}

	return tpoly{
		{ {x,y}, {x+w,y}, {x+w,y+h}, {x,y+h}},
		{x,y}};
}
