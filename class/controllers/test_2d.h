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
	ldv::camera					game_camera;
	room						game_room;
	draw_struct					game_draw_struct;
	player						game_player;
	localization					game_localization;
	std::set<int>					unique_actions;

	//TODO: Remove
	lda::audio_channel				player_channel,
							continuous_channel;


	//TODO: This happens to work, actually... the callbacker need to be 
	//defined only once without the cout.

	struct callbacker:
		public lda::audio_callback_interface
	{
		virtual void		do_callback()
		{
			std::cout<<"This is the callback thing"<<std::endl;
			fnc();
		}
		std::function<void(void)>		fnc;
	};

	//TODO: Any sound player can go like this, actually. When the
	//channel is assigned we can lambda the callbacker so it calls anything
	//that is needed, like, for instance, unlinking the channel. The only
	//thing that's needed is that the player is not a temporary.
	struct sound_player
	{
		lda::audio_channel			channel;
		callbacker				cb;

		void					set(lda::audio_channel& c)
		{
			channel=c;
			cb.fnc=[=](){fuck_the_power();};
			channel.assign_callback_listener(cb);
		}

		void 					fuck_the_power()
		{
			std::cout<<"fuck the power"<<std::endl;
			channel.set_monitoring(false);
			channel.unlink();
		}
	} sp;
};

}

#endif
