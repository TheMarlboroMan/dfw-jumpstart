#include "player.h"

#include "display_defs.h"

using namespace app;

player::player():
	bounding_box{0,0,w,h},
	prev_bounding_box(bounding_box)
{

}

void player::step(float /*delta*/)
{
	//TODO.
}

bool player::is_in_camera(const ldv::rect&) const
{
	//TODO.
	return false;
}

void player::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& sr) const
{
	ds.set_type(app::draw_struct::types::box);
	ds.set_color(ldv::rgb8(0,128,0));
	ds.set_primitive_fill(ldv::polygon_representation::type::fill);
	ds.set_box_location({(int)bounding_box.origin.x, (int)bounding_box.origin.y, bounding_box.w, bounding_box.h});
	ds.rep->draw(scr, cam);

	//Now the animation...
	const auto& frame=sr.get_animation(animation_defs::player).get(player_animation_defs::idle_e).get(0).frame;
	const auto& frect=frame.get_rect();

	ds.set_type(app::draw_struct::types::bitmap);
	ds.set_texture(sr.get_video_resource_manager().get_texture(texture_defs::playersheet));
	ds.set_clip(frect);
	ds.set_location(
		ds.location_projection(
			ds.draw_box_from_spatiable(bounding_box), 
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
