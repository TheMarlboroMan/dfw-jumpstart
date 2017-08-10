#include "draw_struct.h"
#include <templates/compatibility_patches.h>

using namespace app;

draw_struct::draw_struct()
	:rep_bmp(), 
	rep_group({0,0}, true),
	rep_points(ldv::rgba8(0,0,0,255)),
	rep_box(ldv::box_representation::type::fill, {0,0,0,0}, ldv::rgba8(0,0,0,255)),
	rep_line(0,0,0,0, ldv::rgba8(0,0,0,255)),
	rep_polygon(ldv::polygon_representation::type::fill, {{0,0}, {1,0}, {1,2}}, ldv::rgba8(0,0,0,255)), 
	rep(&rep_bmp),
	type(types::bitmap)
{

}

draw_struct::~draw_struct()
{
	rep_group.clear();
}

void draw_struct::set_color(ldv::rgb_color c)
{
	rep_points.set_color(c);
	rep_box.set_color(c);
	rep_line.set_color(c);
	rep_polygon.set_color(c);
}

void draw_struct::set_type(types t)
{
	type=t;

	switch(type)
	{
		case types::bitmap: 
			rep=&rep_bmp;
		break;

		case types::group:
			rep_group.clear();
			rep=&rep_group;
		break;

		case types::points:
			rep_points.clear();
			rep=&rep_points;
		break;

		case types::box:
			rep=&rep_box;
		break;

		case types::line:
			rep=&rep_line;
		break;

		case types::polygon:
			rep=&rep_polygon;
		break;

		case types::external:
			rep=nullptr;
		break;
	}

	if(rep)
	{
		rep->reset_transform();
		set_alpha(255);
		set_blend(ldv::representation::blends::alpha);
	}
}

void draw_struct::set_alpha(unsigned int v) 
{
	if(v > 255) v=255;
	rep->set_alpha(v);
}

void draw_struct::set_blend(ldv::representation::blends v) 
{
	rep->set_blend(v);
}

///////////////////
// Metodos para manipular el grupo de representaciones...

void draw_struct::insert_in_group(ldv::representation * r) 
{
	if(type==types::group)
	{
		rep_group.insert(r);
	}
}

void draw_struct::set_external(ldv::representation& p_rep)
{
	type=types::external;
	rep=&p_rep;
}

void draw_struct::set_primitive_fill(ldv::polygon_representation::type f)
{
	rep_polygon.set_filltype(f);
	rep_box.set_filltype(f);
}

//Gets a rectangle for screen position according to the projection used.

ldv::rect draw_struct::location_projection(const ldv::rect& r, const ldv::rect& f, int dispx, int dispy) const
{
	return ldv::rect(
		r.origin.x+dispx,
		r.origin.y+dispy-r.h, 
		f.w, f.h);
}

ldv::rect draw_struct::draw_box_from_spatiable_box(const ldt::box<float, unsigned int>& box) const
{
	return ldv::rect(
		box.origin.x, //Let narrowing happen.
		box.origin.y,
		box.w, box.h);
}
