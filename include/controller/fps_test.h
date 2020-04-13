#pragma once

//local
#include "states.h"
#include "../app/shared_resources.h"

//libdansdl2

//framework
#include <dfw/controller_interface.h>

//std
#include <cmath>

namespace controller
{

class fps_test:
	public dfw::controller_interface {
	public:

								fps_test(app::shared_resources&);
	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	app::shared_resources&				s_resources;

	//properties
	int						frames,
							frames_accum,
							frames_measured,
							seconds;
};

}
