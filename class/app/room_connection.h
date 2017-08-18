#ifndef ROOM_CONNECTION_H
#define ROOM_CONNECTION_H

//Std
#include <string>

//local
#include "spatiable.h"
#include "game_defs.h"

namespace app
{

//TODO: Yeah... maybe this goes further than being the base of these two.

class room_object:
	public app_interfaces::spatiable
{
	public:

	//////////////////////////
	//Public interface...
	room_object():bounding_box{0,0,0,0}{};
	room_object(app_interfaces::spatiable::t_box b):bounding_box(b) {}

	//////////////////////////
	//Spatiable implementation

	virtual app_interfaces::spatiable::t_box get_box() const {return bounding_box;}
	virtual void 			set_box_x(tpos v) {bounding_box.origin.x=v;}
	virtual void 			set_box_y(tpos v) {bounding_box.origin.y=v;}
	virtual void 			displace_box(tpos x, tpos y) 
	{
		bounding_box.origin.x+=x;
		bounding_box.origin.y+=y;
	}
	virtual void 			set_box_w(tdim v) {bounding_box.w=v;}
	virtual void 			set_box_h(tdim v) {bounding_box.h=v;}

	private:

	app_interfaces::spatiable::t_box	bounding_box;
};

class room_exit:
	public room_object
{
	public:

	room_exit(app_interfaces::spatiable::t_box b, const std::string& m, int tid):
		room_object(b), destination_map(m), terminus_id(tid)
	{

	}

	const std::string&		get_destination_map() const {return destination_map;}
	int				get_terminus_id() const {return terminus_id;}
	

	private:

	std::string			destination_map;
	int				terminus_id;
};

class room_entrance:
	public room_object
{
	public:

	room_entrance(app_interfaces::spatiable::t_point origin, int be, int tid):
		room_object({(tpos)origin.x, (tpos)origin.y, w, h}), 
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
