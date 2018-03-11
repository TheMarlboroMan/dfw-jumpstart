#ifndef CONTROLLER_CONSOLE_H
#define CONTROLLER_CONSOLE_H

//std
#include <cmath>
#include <string>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//tools
#include <class/ttf_manager.h>

//local
#include "states.h"

//app
#include "../app/shared_resources.h"

namespace app
{

class controller_console:
	public dfw::controller_interface
{
	public:

						controller_console(shared_resources&);
	virtual void 				loop(dfw::input& input, float delta, int step);
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

#endif
