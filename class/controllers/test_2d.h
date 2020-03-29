#pragma once

//local
#include "states.h"
#include "signals.h"

//app
#include "../app/room.h"
#include "../app/shared_resources.h"
#include "../app/draw_struct.h"
#include "../app/player.h"
#include "../app/audio_dispatcher.h"
#include "../app/audio_tools.h"

//Tools
#include <tools/i8n.h>

//libdansdl2

//framework
#include <dfw/controller_interface.h>

//std
#include <cmath>
#include <set>

namespace app
{

class controller_test_2d:
	public dfw::controller_interface
{
	public:

						controller_test_2d(shared_resources&, dfw::signal_dispatcher&);

	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const;

	// Action dispatchers.
	void					do_room_change(const std::string&, int);
	void					do_room_change(const room_action_exit&);
	void					do_console_transition(const room_action_console&);
	void					do_arcade_transition(const room_action_arcade&);
	void					do_text_display(const room_action_text&);

	private:
	void					do_trigger(const object_trigger&);
	void					setup_signal_receiver();

	//references...
	shared_resources&				s_resources;

	//broadcasting
	dfw::signal_broadcaster				broadcaster;
	app_receiver					receiver;

	//properties
	ldv::camera					game_camera;
	music_fader					m_fader;
	room						game_room;
	audio_dispatcher				game_audio_dispatcher;
	draw_struct					game_draw_struct;
	player						game_player;
	tools::i8n					game_localization;
	std::set<int>					unique_actions;
};

}
