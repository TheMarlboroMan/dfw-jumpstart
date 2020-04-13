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

namespace controller
{

class test_2d:
	public dfw::controller_interface
{
	public:

								test_2d(app::shared_resources&, dfw::signal_dispatcher&);

	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const;

	// Action dispatchers.
	void					do_room_change(const std::string&, int);
	void					do_room_change(const app::room_action_exit&);
	void					do_console_transition(const app::room_action_console&);
	void					do_arcade_transition(const app::room_action_arcade&);
	void					do_text_display(const app::room_action_text&);

	private:
	void					do_trigger(const app::object_trigger&);
	void					setup_signal_receiver();

	//references...
	app::shared_resources&				s_resources;

	//broadcasting
	dfw::signal_broadcaster				broadcaster;
	app_receiver					receiver;

	//properties
	ldv::camera					game_camera;
	app::music_fader					m_fader;
	app::room						game_room;
	app::audio_dispatcher				game_audio_dispatcher;
	app::draw_struct					game_draw_struct;
	app::player						game_player;
	tools::i8n					game_localization;
	std::set<int>					unique_actions;
};

}
