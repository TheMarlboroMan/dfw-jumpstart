#ifndef CONTROLLER_MENU_H
#define CONTROLLER_MENU_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//tools
#include <templates/options_menu.h>
#include <class/view_composer.h>
#include <class/number_generator.h>

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

	struct bar_struct
	{
		bar_struct(ldv::representation& r, float t, float m):
			rep(r), pos(r.get_position().x),
			time(t), mult(m)
		{}

		void					step(float delta)
		{
			time+=delta;
			pos+=sin(time)*mult;
			rep.go_to({(int)pos, 0});
		}
	
		private:

		ldv::representation&			rep;
		float 					pos, time, mult;

	};

	//references...
	shared_resources&				s_resources;

	//properties
	tools::options_menu<std::string>		menu;
	menu_representation<std::string>		menu_rep;
	tools::view_composer				layout;
	localization					menu_localization;

	std::vector<bar_struct>				menu_decorations;

	struct
	{
		bool					visible,
							changed;
		float					time;
		
		void					step(float delta)
		{
			changed=false;
			time-=delta;
			if(time <= 0.f) toggle();
		}

		void					toggle()
		{
			visible=!visible;
			changed=true;

			if(visible) time=0.08f;
			else
			{
				tools::int_generator g(500, 3000);
				time=(float)g() / 1000.f;
			}
		}
	}						flicker;

	struct
	{
		float					time=0;
		int					val=0;
		void					step(float delta) {val=191+(sin(time+=delta)*64);}
	}						pulse;
};

}

#endif
