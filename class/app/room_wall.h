#ifndef ROOM_WALL_H
#define ROOM_WALL_H

#include "spatiable.h"

namespace app
{

//All walls in a room are simple impassable boxes.

class room_wall:
//TODO: There is no fucking need to have this as a spatiable: this does not move
//and its bounding boxes may trascend the "box". Perhaps we can add a "collides with spatiable"
//method that uses the spatiable bounding box.
//This, however, only sheds light on how limited the interface really is.
//Still, there is no need, as there is a coarse algorithm in place already to
//get the boxes.

//TODO: Maybe EXPAND spatiable to include polygon based ones???.

//TODO: Create static polygon shapes: a box and three triangles.

	public app_interfaces::spatiable
{
	public:

	enum class twall{wall, nwcorner, necorner, secorner, swcorner};

	static const int 			wall_w=32,
						wall_h=32;

						room_wall(int, int, int);

	//////////////////////
	//Implementation of spatiable

	virtual t_box 				get_box() const {return bounding_box;}
	virtual void 				set_box_x(tpos v) {bounding_box.origin.x=v;}
	virtual void 				set_box_y(tpos v) {bounding_box.origin.y=v;}
	virtual void 				displace_box(tpos, tpos) {/*Ignore...*/}
	virtual void 				set_box_w(tdim v) {bounding_box.w=v;}
	virtual void 				set_box_h(tdim v) {bounding_box.h=v;}

	private:

	twall					type;

	//TODO: More like a... polygon, you know? Perhaps?
	//TODO: Or we can have static polygons and move them around.
	app_interfaces::spatiable::t_box	bounding_box; //This is mostly a requirement of a spatiable... which is stupid since this is NOT a spatiable...
};

room_wall::twall room_wall_int_to_type(int);

}

#endif
