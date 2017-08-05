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

						controller_template(ldt::log&);
	virtual void 				preloop(dfw::input& input, float delta, int);
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input& input, float delta, int);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	ldt::log&					log;

	//properties
};

}

#endif
