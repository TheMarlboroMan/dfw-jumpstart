#pragma once

//local
#include "states.h"

//app
#include "../app/shared_resources.h"

//tools
#include <ldtools/ttf_manager.h>

//framework
#include <dfw/controller_interface.h>

//libdansdl2

//std
#include <cmath>
#include <string>

namespace controller
{

class console:
	public dfw::controller_interface {
	public:

								console(app::shared_resources&);
	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&);
	virtual void 				slumber(dfw::input&);
	virtual bool				can_leave_state() const;

	private:

	void					do_color_change(const std::string&, int&, int&, int&);

	//references...
	app::shared_resources&				s_resources;

	//properties
	std::string					current_command;
	std::vector<std::string>			history;
	int						bgc_r, bgc_g, bgc_b, fgc_r, fgc_g, fgc_b;
	float						time;
};

}
