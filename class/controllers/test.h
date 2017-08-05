#ifndef CONTROLLER_TEST_H
#define CONTROLLER_TEST_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//Tools.
#include <class/ttf_manager.h>

//local
#include "states.h"

namespace app
{

class controller_test:
	public dfw::controller_interface
{
	public:

						controller_test(ldt::log&, ldv::resource_manager&, const tools::ttf_manager&);

	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	ldt::log&					log;
	const tools::ttf_manager&			ttf_man;

	//properties
};

}

#endif
