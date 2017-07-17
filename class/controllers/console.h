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

namespace app
{

class controller_console:
	public dfw::controller_interface
{
	public:

						controller_console(ldt::log&, const tools::ttf_manager&, const ldv::resource_manager&);
	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen);
	virtual void 				awake(dfw::input&);
	virtual void 				slumber(dfw::input&);
	virtual bool				can_leave_state() const;

	private:

	void					do_color_change(const std::string&, int&, int&, int&);

	//references...
	ldt::log&					log;
	const tools::ttf_manager&			ttf_man;
	const ldv::resource_manager& 			res_man;

	//properties
	std::string					current_command;
	std::vector<std::string>			history;
	int						bgc_r, bgc_g, bgc_b, fgc_r, fgc_g, fgc_b;
	float						time;
};

}

#endif
