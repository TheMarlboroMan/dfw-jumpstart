#include "motion.h"

using namespace app;

motion::motion(t_vector v)
	:vector(v)
{

}

float motion::integrate_vector(float delta, float &vec, float factor)
{
	float copy=vec;
	vec+=factor * delta;
	float val=copy + vec;
	return val * 0.5 * delta;
}

void motion::add_vector(float c, axis t)
{
	switch(t)
	{
		case axis::x: vector.x+=c; break;
		case axis::y: vector.y+=c; break;
	}
}

void motion::set_vector(float c, axis t)
{
	switch(t)
	{
		case axis::x: vector.x=c; break;
		case axis::y: vector.y=c; break;
	}
}
