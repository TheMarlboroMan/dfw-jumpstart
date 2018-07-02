#ifndef CONTROLLER_STEP_H
#define CONTROLLER_STEP_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>
#include <class/chrono.h>

//local
#include "states.h"
#include "../app/shared_resources.h"

namespace app
{

class controller_step:
	public dfw::controller_interface
{
	public:

						controller_step(shared_resources&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/);
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	//references...
	shared_resources&			s_resources;

	//properties
	struct {int r=128,g=64,b=20;}			color;
	struct {
		float pos, speed, max_limit, elapsed;
		int				grit=10, steps=0;
		tools::chrono			chrono;
	}						data;
};

}

#endif