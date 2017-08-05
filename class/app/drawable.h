#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <video/rect/rect.h>

namespace app_interfaces
{

class drawable
{
	public:

	//These should return the x position and the bottom y position,
	//given the perspective and shit.
	virtual float		get_ordering_x() const=0;
	virtual float		get_ordering_y() const=0;
	virtual bool		is_in_camera(const ldv::rect&) const=0;

	//TODO: We need, of course, a pure "draw" function that will receive
	//the screen, tilesets and shit.
};

//Standalone function to see if a drawable must be drawn before another.
bool drawable_order(const drawable* a, const drawable* b);

}

#endif
