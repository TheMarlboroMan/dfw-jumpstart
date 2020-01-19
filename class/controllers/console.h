#pragma once

//local
#include "states.h"

//app
#include "../app/shared_resources.h"

//tools
#include <class/ttf_manager.h>

//framework
#include <class/controller_interface.h>

//libdansdl2
#include <def_video.h>

//std
#include <cmath>
#include <string>

namespace app
{

class controller_console:
	public dfw::controller_interface {
	public:

						controller_console(shared_resources&);
	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&);
	virtual void 				slumber(dfw::input&);
	virtual bool				can_leave_state() const;

	private:

	void					do_color_change(const std::string&, int&, int&, int&);

	//references...
	shared_resources&				s_resources;

	//properties
	std::string					current_command;
	std::vector<std::string>			history;
	int						bgc_r, bgc_g, bgc_b, fgc_r, fgc_g, fgc_b;
	float						time;
};

}
