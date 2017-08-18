#include "spatiable.h"

using namespace app_interfaces;

bool spatiable::is_colliding_with(const t_box& e, bool roce_es_colision) const
{
	return get_box().collides_with(e, roce_es_colision);
}

bool spatiable::is_colliding_with(const spatiable& e, bool roce_es_colision) const
{
	return get_box().collides_with(e.get_box(), roce_es_colision);
}

spatiable::t_box spatiable::get_box_displaced(tpos x, tpos y) const
{
	auto c=get_box();
	c.origin.x+=x;
	c.origin.y+=y;
	return c;
}

void spatiable::set_position(const spatiable& e)
{
	set_position({e.get_spatiable_x(), e.get_spatiable_y()});
}

void spatiable::set_position(t_point p)
{
	set_box_x(p.x);
	set_box_y(p.y);
}

/*
ldt::vector_2d_screen<tpos> spatiable::screen_vector_for(const spatiable& a, const spatiable& b) const
{
	tpos ax=a.get_spatiable_cx(),
		ay=a.get_spatiable_cy(),
		bx=b.get_spatiable_cx(),
		by=b.get_spatiable_cy();

	return ldt::for_points_screen<tpos>(bx, by, ax, ay);
}

tpos spatiable::angle_for(const spatiable& a, const spatiable& b) const
{
	return screen_vector_for(a, b).angle_deg();
}

ldt::vector_2d_cartesian<tpos> spatiable::cartesian_vector_for(const spatiable& a, const spatiable& b) const
{
	tpos ax=a.get_spatiable_cx(),
		ay=a.get_spatiable_cy(),
		bx=b.get_spatiable_cx(),
		by=b.get_spatiable_cy();

	return ldt::for_points_cartesian<tpos>(bx, by, ax, ay);
}

tpos spatiable::cartesian_angle_for(const spatiable& a, const spatiable& b) const
{
	return cartesian_vector_for(a, b).angle_deg();
}
*/

void spatiable::center_on(const spatiable& s)
{
	center_on({s.get_spatiable_cx(), s.get_spatiable_cy()});
}

void spatiable::center_on(t_point p)
{
	tpos x=p.x-(get_spatiable_w()/2),
		y=p.y-(get_spatiable_h()/2);
	set_position({x, y});
}
