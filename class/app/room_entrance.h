#pragma once 

//local
#include "game_defs.h"
#include "room_object_box.h"

//Std
#include <string>

namespace app
{

class room_entrance:
	public room_object_box
{
	public:

	room_entrance(tpoint origin, int be, int tid):
		room_object_box({origin.x, origin.y, w, h}), 
		val_bearing(be), terminus_id(tid)
	{}

	int				get_terminus_id() const {return terminus_id;}
	bearing				get_bearing() const {return val_bearing;}

	private:

	static const size_t		w=32, h=32;

	bearing				val_bearing;
	int				terminus_id;
};

}
