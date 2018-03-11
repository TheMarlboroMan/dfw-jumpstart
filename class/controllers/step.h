#ifndef CONTROLLER_STEP_H
#define CONTROLLER_STEP_H

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

class controller_step:
	public dfw::controller_interface
{
	public:

						controller_step(shared_resources&);
	virtual void 				loop(dfw::input&, float, int);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/) {}
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	//references...
	shared_resources&			s_resources;

	//properties
	struct {int r=128,g=64,b=20;}			color;
};

}

#endif
