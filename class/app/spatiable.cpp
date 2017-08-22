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

spatiable::t_box app_interfaces::coarse_bounding_box(const spatiable::t_poly& p)
{
	auto &vt=p.get_vertexes();

	tpos 	minx=vt[0].x, 	maxx=minx,
		miny=vt[0].y,	maxy=miny;

	auto cmp=[](tpos src, tpos& mind, tpos& maxd)
	{
		if(src < mind) mind=src;
		if(src > maxd) maxd=src;
	};

	for(const auto& v : vt)
	{
		cmp(v.x, minx, maxx);
		cmp(v.y, miny, maxy);
	}

	return spatiable::t_box(minx, miny, maxx-minx, maxy-miny);
}

spatiable::t_box app_interfaces::coarse_bounding_box(const spatiable& s) 
{
	return app_interfaces::coarse_bounding_box(s.get_poly());
}
