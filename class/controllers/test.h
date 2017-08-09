#ifndef CONTROLLER_TEST_H
#define CONTROLLER_TEST_H

//std
#include <cmath>

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
/*
class game_tile:
	public spatiable,
	public drawable
{
	//This should simply represent the game state, would go like:
	x, y (in grid... maybe not), type (indicating what it does).
};

*/

namespace app
{

class controller_test:
	public dfw::controller_interface
{
	public:

						controller_test(shared_resources&);

	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	void					do_room_change(const std::string&, int);

	//references...
	shared_resources&				s_resources;

	//properties
	ldv::camera					game_camera;
	room						game_room;
	draw_struct					game_draw_struct;
	player						game_player;
};

}

#endif
