#ifndef CONTROLLER_TEST_2D_TEXT_H
#define CONTROLLER_TEST_2D_TEXT_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"
#include "../app/shared_resources.h"


namespace app
{

class controller_test_2d_text:
	public dfw::controller_interface
{
	public:

						controller_test_2d_text(shared_resources&);
	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input&, float);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const {return true;}

	virtual void				request_draw(dfw::controller_view_manager&); 

	virtual bool				is_receiver() const {return true;}
	virtual void				receive(const dfw::broadcast_message& msg)
	{
		std::cout<<"this is text, copy:"<<msg.tok["data"]["txt"]<<std::endl;
	}

	private:



	//references...
	shared_resources&				s_resources;
};

}

#endif
