#ifndef ROOM_WALL_H
#define ROOM_WALL_H

#include "spatiable.h"

namespace app
{

//All walls in a room are simple impassable boxes.

class room_wall:
	public app_interfaces::spatiable
{
	public:

	enum class twall{full, nwcorner, necorner, secorner, swcorner, wthin, ethin};
	enum indexes{shape_full, shape_nwcorner, shape_necorner, shape_secorner, shape_swcorner, shape_wthin, shape_ethin};

	static const int 			wall_w=32,
						wall_h=32;
	static std::vector<tpoly>		shapes;

						room_wall(int, int, int);

	// Implementation of spatiable

	virtual const tpoly&			get_poly() const;
	virtual tpoly *				get_poly_ptr();

	private:

	twall					type;
	int					x, y;
};

room_wall::twall room_wall_int_to_type(int);

}

#endif
