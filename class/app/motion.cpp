#include "motion.h"

using namespace app;

motion::motion(tvector v)
	:vector(v)
{

}

void motion::add_vector(tpos c, axis t)
{
	switch(t)
	{
		case axis::x: vector.x+=c; break;
		case axis::y: vector.y+=c; break;
	}
}

void motion::set_vector(tpos c, axis t)
{
	switch(t)
	{
		case axis::x: vector.x=c; break;
		case axis::y: vector.y=c; break;
	}
}

tpos motion::get_vector(axis t)
{
	switch(t)
	{
		case axis::x: return vector.x; break;
		case axis::y: return vector.y; break;
	}

	return 0; //Stupid compiler complaining about impossible cases... That was an enum class!.
}
