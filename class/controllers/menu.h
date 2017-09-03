#ifndef CONTROLLER_MENU_H
#define CONTROLLER_MENU_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//tools
#include <templates/options_menu.h>
#include <class/view_composer.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"
#include "../app/shared_resources.h"
#include "../app/localization.h"
#include "../app/menu_representation.h"

namespace app
{

class controller_menu:
	public dfw::controller_interface
{
	public:

						controller_menu(shared_resources&);
	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	void					mount_menu();

	//references...
	shared_resources&				s_resources;

	//properties
	tools::options_menu<std::string>		menu;
	menu_representation<std::string>		menu_rep;
	tools::view_composer				layout;
	localization					menu_localization;
};

}

#endif
