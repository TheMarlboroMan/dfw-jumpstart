#include "player.h"

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

void player::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& /*sr*/) const
{
	ds.set_type(app::draw_struct::types::box);
	ds.set_color(ldv::rgb8(0,128,0));
	ds.set_primitive_fill(ldv::polygon_representation::type::fill);
	ds.set_box_location({(int)bounding_box.origin.x, (int)bounding_box.origin.y, bounding_box.w, bounding_box.h});
	ds.rep->draw(scr, cam);
}
