#pragma once

//local
#include "states.h"
#include "../app/shared_resources.h"

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//std
#include <cmath>

namespace app
{

class controller_fps_test:
	public dfw::controller_interface {
	public:

						controller_fps_test(shared_resources&);
	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	shared_resources&				s_resources;

	//properties
	int						frames,
							frames_accum,
							frames_measured,
							seconds;
};

}
