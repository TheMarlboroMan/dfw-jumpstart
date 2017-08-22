#include "spatiable.h"

using namespace app_interfaces;

bool spatiable::is_colliding_with(const spatiable& o) const
{
	return ldt::SAT_collision_check(get_poly(), o.get_poly());
}

void spatiable::move_by(t_point p)
{
	get_poly_ptr()->move(p);
}

void spatiable::center_on(t_point p)
{
	get_poly_ptr()->center_in(p);
}

void spatiable::center_on(t_poly p)
{
	//Now... p.get_center may not be the spatial center.
	get_poly_ptr()->center_in(p.get_center());
}

spatiable::t_box app_interfaces::box_from_polygon(const spatiable::t_poly& p)
{
	auto &vt=p.get_vertexes();
	tpos x=vt[0].x, y=vt[0].y;
	tdim w=0., h=0.;

	for(const auto& v : vt)
	{
		if(v.x < x) x=v.x;
		if(v.y < y) y=v.y;

		if(v.x-x > w) w=v.x-x;
		if(v.y-y > h) h=v.y-y;
	}

	return spatiable::t_box{x, y, w, h};
}

spatiable::t_box app_interfaces::box_from_spatiable(const spatiable& s) 
{
	return app_interfaces::box_from_polygon(s.get_poly());
}
