#include "room_wall.h"

#include <stdexcept>

using namespace app;

std::vector<tpoly>	room_wall::shapes={
	//full
	{ { {0.,0.}, {32.,0.}, {32.,32.}, {0.,32.} },
		{0., 0.}
	},
	//nwcorner
	{ { {0.,0.}, {32.0,0.}, {0.,32.} },
		{0., 0.}
	},
	//necorner
	{ { {0.,0.}, {32.0,0.}, {32.,32.} },
		{0., 0.}
	},
	//secorner
	{ { {32.,0.}, {32.,32.}, {0.,32.} },
		{32., 0.}
	},
	//swcorner
	{ { {0.,0.}, {32.,32.}, {0.,32.} },
		{0., 0.}
	},
	//wthin
	{ { {0.,0.}, {16.,0.}, {16.,32.}, {0.,32.} },
		{0., 0.}
	},
	//ethin
	{ { {24.,0.}, {32.,0.}, {32.,32.}, {24.,32.} },
		{24., 0.}
	}
};

room_wall::room_wall(int cellx, int celly, int t)
	:type(room_wall_int_to_type(t)), x(cellx), y(celly) {


}

room_wall::twall app::room_wall_int_to_type(int t) {
	switch(t) {
		case 1: 	return room_wall::twall::full; break;
		case 2: 	return room_wall::twall::nwcorner; break;
		case 3: 	return room_wall::twall::necorner; break;
		case 4: 	return room_wall::twall::secorner; break;
		case 5: 	return room_wall::twall::swcorner; break;
		case 6: 	return room_wall::twall::wthin; break;
		case 7: 	return room_wall::twall::ethin; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	return room_wall::twall::full;
}

const tpoly& room_wall::get_poly() const {
	//Fuck lol.
	return *(const_cast<room_wall*>(this)->get_poly_ptr());
}

tpoly * room_wall::get_poly_ptr() {

	tpoly * res=&shapes[shape_full];

	switch(type) 	{
		case room_wall::twall::full: 		res=&shapes[shape_full]; break;
		case room_wall::twall::nwcorner: 	res=&shapes[shape_nwcorner]; break;
		case room_wall::twall::necorner: 	res=&shapes[shape_necorner]; break;
		case room_wall::twall::secorner: 	res=&shapes[shape_secorner]; break;
		case room_wall::twall::swcorner: 	res=&shapes[shape_swcorner]; break;
		case room_wall::twall::wthin: 		res=&shapes[shape_wthin]; break;
		case room_wall::twall::ethin: 		res=&shapes[shape_ethin]; break;
		default:
			throw std::runtime_error("Invalid wall type");
		break;
	}

	//Assume the first vertex as the center and be done: all shapes are 
	//defined so the first vertex and the center are the same.
	double 	cx=(x*wall_w), //+(wall_w/2),
		cy=(y*wall_h); //+(wall_h/2);

	res->center_in({cx, cy});

	return res;
}
