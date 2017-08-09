#include "motion.h"

using namespace app;

motion::motion(t_vector v)
	:vector(v)
{

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

float motion::get_vector(axis t)
{
	switch(t)
	{
		case axis::x: return vector.x; break;
		case axis::y: return vector.y; break;
	}

	return 0.f; //Stupid compiler complaining about impossible cases... That was an enum class!.
}
