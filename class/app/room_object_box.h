#ifndef ROOM_OBJECT_BOX_H
#define ROOM_OBJECT_BOX_H

//local
#include "spatiable.h"

namespace app
{

class room_object_box:
	public app_interfaces::spatiable
{
	public:

	// Public interface...

	room_object_box(const app_interfaces::spatiable::t_box& b)
		:polygon{
			{ 
				{b.origin.x, b.origin.y}, 
				{b.origin.x+(tpos)b.w, b.origin.y}, 
				{b.origin.x+(tpos)b.w, b.origin.y+(tpos)b.h}, 
				{b.origin.x, b.origin.y+(tpos)b.h} },
			{b.origin.x+((tpos)b.w/2), b.origin.y+((tpos)b.h/2)}}
	{

	}

	bool				operator==(const room_object_box& o) const {return polygon==o.polygon;}

	//Spatiable implementation

	virtual const t_poly&		get_poly() const {return polygon;}
	virtual t_poly *		get_poly_ptr() {return &polygon;}

	private:

	app_interfaces::spatiable::t_poly	polygon;
};
}

#endif
