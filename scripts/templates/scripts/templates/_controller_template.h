#pragma once

#include "states.h"
#include <dfw/controller_interface.h>
#include <lm/logger.h>
#include <cmath>

#include "app/service_provider.h"

namespace controller {

class _template:
	public dfw::controller_interface {

	public:

	                            _template(app::service_provider&);
	virtual void                loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void                draw(ldv::screen&, int);
	virtual void                awake(dfw::input& /*input*/) {}
	virtual void                slumber(dfw::input& /*input*/) {}
	virtual bool                can_leave_state() const {return true;}

	private:

	//references...
	lm::logger&                 log;

	//properties
};

}
