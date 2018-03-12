#ifndef CONTROLLER_TEST_2D_TEXT_H
#define CONTROLLER_TEST_2D_TEXT_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//tools
#include <class/view_composer.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"
#include "signals.h"
#include "../app/shared_resources.h"


namespace app
{

class controller_test_2d_text:
	public dfw::controller_interface
{
	public:

						controller_test_2d_text(shared_resources&, dfw::signal_dispatcher&);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const {return true;}

	virtual void				request_draw(dfw::controller_view_manager&); 
	virtual void				receive(const dfw::broadcast_signal&);

	private:

	void					redraw_text();
	void					setup_signal_receiver();

	//references...
	shared_resources&				s_resources;

	//properties.
	tools::view_composer				layout;
	app_receiver					receiver;
	enum class tstates{dictate, wait} 		state;
	std::vector<std::string>			text_buffer;
	size_t						current_index;
	float						time_text, time_blink;
				
};

}

#endif
