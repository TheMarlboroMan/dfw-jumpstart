#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "draw_struct.h"
#include "shared_resources.h"
#include "app_defs.h"
#include <video/rect/rect.h>

namespace app_interfaces
{

class drawable
{
	public:

	//These should return the x position and the bottom y position,
	//given the perspective and shit... Assumes that all representations can fit
	//within boxes.

	virtual app::tpos	get_ordering_x() const=0;
	virtual app::tpos	get_ordering_y() const=0;
	virtual bool		is_in_camera(const ldv::rect&) const=0;
	virtual void		draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const=0;
};

//Standalone function to see if a drawable must be drawn before another.
bool drawable_order(const drawable* a, const drawable* b);

}

#endif
