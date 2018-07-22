#ifndef CONTROLLER_TEMPLATE_H
#define CONTROLLER_TEMPLATE_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"

namespace app
{

class controller_template:
	public dfw::controller_interface
{
	public:

						controller_template(tools::log&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/) {}
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	//references...
	tools::log&					log;

	//properties
};

}

#endif
