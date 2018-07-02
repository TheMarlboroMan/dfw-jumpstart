#include "spatiable.h"

using namespace app_interfaces;

bool spatiable::is_colliding_with(const spatiable& o) const
{
	return ldt::SAT_collision_check(get_poly(), o.get_poly());
}

bool spatiable::is_colliding_with(const app::tpoly& o) const
{
	return ldt::SAT_collision_check(get_poly(), o);
}

void spatiable::move_by(app::tpoint p)
{
	get_poly_ptr()->move(p);
}

void spatiable::center_on(app::tpoint p)
{
	get_poly_ptr()->center_in(p);
}

void spatiable::center_on(app::tpoly p)
{
	//Now... p.get_centroid may not be the spatial center.
	get_poly_ptr()->center_in(p.get_centroid());
}

app::tbox app_interfaces::coarse_bounding_box(const app::tpoly& p)
{
	auto &vt=p.get_vertexes();

	app::tpos 	minx=vt[0].x, 	maxx=minx,
			miny=vt[0].y,	maxy=miny;

	auto cmp=[](app::tpos src, app::tpos& mind, app::tpos& maxd) {
		if(src < mind) mind=src;
		if(src > maxd) maxd=src;
	};

	for(const auto& v : vt) {
		cmp(v.x, minx, maxx);
		cmp(v.y, miny, maxy);
	}

//if(3==vt.size()) {
//	std::cout<<minx<<" "<<miny<<" "<<maxx<<" "<<maxy<<std::endl;
//}	

	return app::tbox(minx, miny, maxx-minx, maxy-miny);
}

app::tbox app_interfaces::coarse_bounding_box(const spatiable& s) 
{
	return app_interfaces::coarse_bounding_box(s.get_poly());
}
