#include "room_wall.h"

#include <stdexcept>

using namespace app;

std::vector<app_interfaces::spatiable::t_poly>	room_wall::shapes=
{
	//full
	{ 
		{ {0.,0.}, {32.,0.}, {32.,32.}, {0.,32.} },
		{0., 0.}
	},
	//nwcorner
	{
		{ {0.,0.}, {32.0,0.}, {0.,32.} },
		{0., 0.}
	},
	//necorner
	{
		{ {0.,0.}, {32.0,0.}, {32.,32.} },
		{0., 0.}
	},
	//secorner
	{
		{ {32.,0.}, {32.,32.}, {0.,32.} },
		{0., 0.}
	},
	//swcorner
	{
		{ {0.,0.}, {32.,32.}, {0.,32.} },
		{0., 0.}
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
	switch(type)
	{
		case room_wall::twall::full: 		return &shapes[shape_full]; break;
		case room_wall::twall::nwcorner: 	return &shapes[shape_nwcorner]; break;
		case room_wall::twall::necorner: 	return &shapes[shape_necorner]; break;
		case room_wall::twall::secorner: 	return &shapes[shape_secorner]; break;
		case room_wall::twall::swcorner: 	return &shapes[shape_swcorner]; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	return &shapes[shape_full];
}
