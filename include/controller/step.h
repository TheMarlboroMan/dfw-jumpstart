#pragma once

//local
#include "states.h"
#include "../app/shared_resources.h"

//Tools
#include <tools/chrono.h>

//framework
#include <dfw/controller_interface.h>

//libdansdl2

//std
#include <cmath>

namespace controller
{

class step:
	public dfw::controller_interface
{
	public:

								step(app::shared_resources&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/);
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	//references...
	app::shared_resources&			s_resources;

	//properties
	struct {int r=128,g=64,b=20;}			color;
	struct {
		float pos, speed, max_limit, elapsed;
		int				grit=10, steps=0;
		tools::chrono			chrono;
	}						data;
};

}
