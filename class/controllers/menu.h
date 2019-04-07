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
#include <class/i8n.h>

//framework
#include <class/controller_interface.h>
#include <class/signal_broadcasting.h>

//local
#include "states.h"
#include "../app/shared_resources.h"
#include "../app/menu_representation.h"
#include "../dfwimpl/app_config.h"

namespace app
{

class controller_menu:
	public dfw::controller_interface
{
	public:

						controller_menu(shared_resources&, dfw::signal_dispatcher&, app_config&);
	virtual void 				loop(dfw::input& input, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const {return true;}
	
	void					set_continue_state(int v) {continue_state=v;}

	private:

	void					create_functions();
	void					mount_menus();
	void					mount_layout();
	void					choose_current_menu(menu_representation<std::string>&);
	void					do_main_menu_input(dfw::input&);
	void					do_options_menu_input(dfw::input&, float);
	void					do_controls_menu_input(dfw::input&);
	std::string				translate_input(const dfw::input_description&);
	void					update_options_value(const std::string&);
	void					learn_control(dfw::input&);
	void					restore_default_controls(dfw::input&);

	//This encapsulates a representation and its logic: the background moving bars.
	struct bar_struct
	{
		bar_struct(ldv::representation& r, float t, float m):
			rep(r), pos(r.get_position().x),time(t), mult(m)
		{}

		void					step(float delta)
		{
			pos+=sin(time+=delta)*mult;
			rep.go_to({(int)pos, 0});
		}

		private:

		ldv::representation&			rep;
		float 					pos, time, mult;

	};

	enum {rf_txt, rf_empty};
	enum {df_txt_right_single, df_txt_left_composite, df_txt_right_composite, df_empty};
	enum {sf_pulse, sf_empty};

	//references...
	shared_resources&				s_resources;
	app_config&					config;

	//Broadcasting...
	dfw::signal_broadcaster				broadcaster;

	std::map<int, menu_representation<std::string>::tfunc_register>	register_funcs;
	std::map<int, menu_representation<std::string>::tfunc_draw>	draw_funcs;
	std::map<int, menu_representation<std::string>::tfunc_step>	step_funcs;

	tools::options_menu<std::string>		main_menu,
							options_menu,
							controls_menu;

	menu_representation<std::string>		main_menu_rep,
							options_menu_rep,
							controls_menu_rep;

	menu_representation<std::string> *		current_menu_ptr=nullptr;
	tools::view_composer				layout;
	tools::i8n					menu_localization;
	std::vector<bar_struct>				menu_decorations; //These are the background moving bars...
	float						key_held_time;
	bool						started=false;	//Just controls if "continue" can be selected.
	int						continue_state=0; //State to be selected when continuing...

	//A few single-use structs...

	struct
	{
		bool					visible, changed;
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
			tools::int_generator g(visible ? 70 : 500 , visible ? 80 : 3000);
			time=(float)g() / 1000.f;
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
