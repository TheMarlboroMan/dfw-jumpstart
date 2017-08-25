#ifndef CONTROLLER_TEST_2D_H
#define CONTROLLER_TEST_2D_H

//std
#include <cmath>
#include <set>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"

//app
#include "../app/room.h"
#include "../app/shared_resources.h"
#include "../app/draw_struct.h"
#include "../app/player.h"
#include "../app/localization.h"

namespace app
{

class controller_test_2d:
	public dfw::controller_interface
{
	public:

						controller_test_2d(shared_resources&);

	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const;

	// message broadcasting
	virtual bool				is_broadcaster() const {return true;}

	private:

	void					do_room_change(const std::string&, int);
	void					do_touch_trigger(const object_trigger&);

	//references...
	shared_resources&				s_resources;

	//properties
	ldv::camera					game_camera;
	room						game_room;
	draw_struct					game_draw_struct;
	player						game_player;
	localization					game_localization;
	std::set<std::string>				unique_triggers;
};

}

#endif
