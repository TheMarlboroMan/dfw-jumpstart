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

	static const int 			wall_w=32,
						wall_h=32;

						room_wall(int, int);

	//////////////////////
	//Implementation of spatiable

	virtual t_box 				get_box() const {return bounding_box;}
	virtual void 				set_box_x(float v) {bounding_box.origin.x=v;}
	virtual void 				set_box_y(float v) {bounding_box.origin.y=v;}
	virtual void 				displace_box(float, float) {/*Ignore...*/}
	virtual void 				set_box_w(unsigned int v) {bounding_box.w=v;}
	virtual void 				set_box_h(unsigned int v) {bounding_box.h=v;}

	private:

	app_interfaces::spatiable::t_box	bounding_box;


};

}

#endif
