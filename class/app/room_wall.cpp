#include "room_wall.h"

#include <stdexcept>

using namespace app;

room_wall::room_wall(int cellx, int celly, int t)
	:type(room_wall_int_to_type(t)), 
	bounding_box(cellx*wall_w, celly*wall_h, wall_w, wall_h)
{

}

room_wall::twall app::room_wall_int_to_type(int t)
{
	switch(t)
	{
		case 1: 	return room_wall::twall::wall; break;
		case 2: 	return room_wall::twall::nwcorner; break;
		case 3: 	return room_wall::twall::necorner; break;
		case 4: 	return room_wall::twall::secorner; break;
		case 5: 	return room_wall::twall::swcorner; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	return room_wall::twall::wall;
}
