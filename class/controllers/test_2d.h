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
#include "../app/audio_dispatcher.h"

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

	// Action dispatchers.
	void					do_room_change(const std::string&, int);
	void					do_room_change(const room_action_exit&);
	void					do_console_transition(const room_action_console&);
	void					do_arcade_transition(const room_action_arcade&);
	void					do_text_display(const room_action_text&);

	private:
	void					do_trigger(const object_trigger&);

	//references...
	shared_resources&				s_resources;

	//properties
	audio_dispatcher				game_audio_dispatcher;
	ldv::camera					game_camera;
	room						game_room;
	draw_struct					game_draw_struct;
	player						game_player;
	localization					game_localization;
	std::set<int>					unique_actions;
};

}
#endif
