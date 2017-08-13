#include "player.h"

#include "display_defs.h"

using namespace app;

player::player():
	bounding_box{0,0,w,h},
	prev_bounding_box(bounding_box),
	player_bearing{2},
	walk_time{0.f}
{

}

void player::step(float delta)
{
	if(motion_data.has_motion()) walk_time+=delta;
	else walk_time=0.f;
	//TODO.
}

bool player::is_in_camera(const ldv::rect&) const
{
	//TODO.
	return false;
}

void player::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& sr) const
{
	//Bounding box.
	ds.set_type(app::draw_struct::types::box);
	ds.set_color(ldv::rgb8(255,0,0));
	ds.set_alpha(128);
	ds.set_primitive_fill(ldv::polygon_representation::type::fill);
	ds.set_box_location({(int)bounding_box.origin.x, (int)bounding_box.origin.y, bounding_box.w, bounding_box.h});
	ds.rep->draw(scr, cam);

	//Now the animation...
	const auto& frame=sr.get_animation(animation_defs::player).get(choose_animation_frame()).get_for_time(walk_time).frame;
	const auto& frect=frame.get_rect();

	ds.set_type(app::draw_struct::types::bitmap);
	ds.set_texture(sr.get_video_resource_manager().get_texture(texture_defs::playersheet));
	ds.set_clip(frect);
	ds.set_location(
		ds.location_projection(
			ds.draw_box_from_spatiable_box(bounding_box), 
			frect, frame.disp_x, frame.disp_y));
	ds.rep->draw(scr, cam);
}

//Receive game input and alter its internal representation as a consequence.

void player::set_input(game_input gi)
{
	if(!gi.x && !gi.y)
	{
		motion_data.set_vector({0.f, 0.f});
	}
	else
	{
		motion_data.set_vector(gi.x, motion::axis::x);
		motion_data.set_vector(gi.y, motion::axis::y);

		//Actually, this is an overloaded operator.
		player_bearing=motion_data.get_vector();
	}
}

void player::integrate_motion(float delta, motion::axis axis)
{
	prev_bounding_box=bounding_box;
	float v=motion_data.get_vector(axis);
	float nv=(v*delta)*speed;

	switch(axis)
	{
		case motion::axis::x:
			set_box_x(get_spatiable_x()+nv);
		break;
		case motion::axis::y:
			set_box_y(get_spatiable_y()+nv);
		break;
	}
}

//This would go right in spatiable if we could have a callback for the vector...

void player::adjust_collision_horizontal(const spatiable& o)
{
/*
	//TODO. Implement in terms of spatiable: have two versions.
	Funny thing... this would probably be a heck of a lot faster and would dispose of the previous bounding boxes.
	The only drawback?. Could not be implemented as part of spatiable.

	float v=motion_data.get_vector_x();
	if(v > 0.f) 		set_box_x(o.get_spatiable_x()-get_spatiable_w());
	else if(v < 0.f)	set_box_x(o.get_spatiable_ex());
*/

	if(o.is_left_of(prev_bounding_box))		set_box_x(o.get_spatiable_ex());
	else if(o.is_right_of(prev_bounding_box))	set_box_x(o.get_spatiable_x()-get_spatiable_w());

	motion_data.set_vector(0.f, motion::axis::x);
}

void player::adjust_collision_vertical(const spatiable& o)
{
	if(o.is_over(prev_bounding_box))		set_box_y(o.get_spatiable_ey());
	else if(o.is_under(prev_bounding_box))		set_box_y(o.get_spatiable_y()-get_spatiable_h());

	motion_data.set_vector(0.f, motion::axis::y);
}

int player::choose_animation_frame() const
{
	switch(player_bearing())
	{
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
