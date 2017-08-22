#include "room_wall.h"

#include <stdexcept>

using namespace app;

std::vector<app_interfaces::spatiable::t_poly>	room_wall::shapes=
{
	//full
	{ 
		{ {0.,0.}, {32.,0.}, {32.,32.}, {0.,32.} },
		{16., 16.}
	},
	//nwcorner
	{
		{ {0.,0.}, {32.0,0.}, {0.,32.} },
		{16., 16.}
	},
	//necorner
	{
		{ {0.,0.}, {32.0,0.}, {32.,32.} },
		{16., 16.}
	},
	//secorner
	{
		{ {32.,0.}, {32.,32.}, {0.,32.} },
		{16., 16.}
	},
	//swcorner
	{
		{ {0.,0.}, {32.,32.}, {0.,32.} },
		{16., 16.}
	}
};

room_wall::room_wall(int cellx, int celly, int t)
	:type(room_wall_int_to_type(t)), x(cellx), y(celly)
{

}

room_wall::twall app::room_wall_int_to_type(int t)
{
	switch(t)
	{
		case 1: 	return room_wall::twall::full; break;
		case 2: 	return room_wall::twall::nwcorner; break;
		case 3: 	return room_wall::twall::necorner; break;
		case 4: 	return room_wall::twall::secorner; break;
		case 5: 	return room_wall::twall::swcorner; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	return room_wall::twall::full;
}

const app_interfaces::spatiable::t_poly& room_wall::get_poly() const
{
	//Fuck lol.
	return *(const_cast<room_wall*>(this)->get_poly_ptr());
}

app_interfaces::spatiable::t_poly * room_wall::get_poly_ptr()
{
	app_interfaces::spatiable::t_poly * res=&shapes[shape_full];

	switch(type)
	{
		case room_wall::twall::full: 	res=&shapes[shape_full]; break;
		case room_wall::twall::nwcorner: res=&shapes[shape_nwcorner]; break;
		case room_wall::twall::necorner: res=&shapes[shape_necorner]; break;
		case room_wall::twall::secorner: res=&shapes[shape_secorner]; break;
		case room_wall::twall::swcorner: res=&shapes[shape_swcorner]; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	//And now, position on its place we know where the center is so...
	double 	cx=(x*wall_w)+(wall_w/2),
		cy=(y*wall_h)+(wall_h/2);
	res->center_in({cx, cy});

	return res;
}
