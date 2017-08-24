#ifndef ROOM_CONNECTION_H
#define ROOM_CONNECTION_H

//Std
#include <string>

//local
#include "game_defs.h"
#include "room_object_box.h"

namespace app
{

class room_exit:
	public room_object_box
{
	public:

	room_exit(const app_interfaces::spatiable::t_box& b, const std::string& m, int tid):
		room_object_box(b), destination_map(m), terminus_id(tid)
	{

	}

	const std::string&		get_destination_map() const {return destination_map;}
	int				get_terminus_id() const {return terminus_id;}
	

	private:

	std::string			destination_map;
	int				terminus_id;
};

class room_entrance:
	public room_object_box
{
	public:

	room_entrance(app_interfaces::spatiable::t_point origin, int be, int tid):
		room_object_box({(tpos)origin.x, (tpos)origin.y, w, h}), 
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

#endif
