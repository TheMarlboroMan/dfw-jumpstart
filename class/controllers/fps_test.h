#ifndef CONTROLLER_FPS_TEST_H
#define CONTROLLER_FPS_TEST_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"
#include "../app/shared_resources.h"

namespace app
{

class controller_fps_test:
	public dfw::controller_interface
{
	public:

						controller_fps_test(shared_resources&);
	virtual void 				preloop(dfw::input& input, float delta, int);
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input& input, float delta, int);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	shared_resources&				s_resources;

	//properties
	int						frames,
							seconds;
};

}

#endif
