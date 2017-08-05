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

spatiable::t_box spatiable::get_box_displaced(float x, float y) const
{
	auto c=get_box();
	c.origin.x+=x;
	c.origin.y+=y;
	return c;
}

void spatiable::set_position(const spatiable& e)
{
	set_position(e.get_spatiable_x(), e.get_spatiable_y());
}

void spatiable::set_position(float x, float y)
{
	set_box_x(x);
	set_box_y(y);
}

ldt::vector_2d_screen<float> spatiable::screen_vector_for(const spatiable& a, const spatiable& b) const
{
	float ax=a.get_spatiable_cx();
	float ay=a.get_spatiable_cy();
	float bx=b.get_spatiable_cx();
	float by=b.get_spatiable_cy();

	return ldt::for_points_screen<float>(bx, by, ax, ay);
}

float spatiable::angle_for(const spatiable& a, const spatiable& b) const
{
	return screen_vector_for(a, b).angle_deg();
}

ldt::vector_2d_cartesian<float> spatiable::cartesian_vector_for(const spatiable& a, const spatiable& b) const
{
	float ax=a.get_spatiable_cx();
	float ay=a.get_spatiable_cy();
	float bx=b.get_spatiable_cx();
	float by=b.get_spatiable_cy();

	return ldt::for_points_cartesian<float>(bx, by, ax, ay);
}

float spatiable::cartesian_angle_for(const spatiable& a, const spatiable& b) const
{
	return cartesian_vector_for(a, b).angle_deg();
}
