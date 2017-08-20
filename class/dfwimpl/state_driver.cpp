#include "state_driver.h"

//std
#include <algorithm>

//tools
#include <class/dnot_parser.h>
#include <source/string_utils.h>

#ifdef WDEBUG_CODE
#include "../input.h"
#endif

using namespace app;

extern ldt::log LOG;

state_driver::state_driver(dfw::kernel& kernel, app::app_config& c)
	:state_driver_interface(t_states::state_main, std::function<bool(int)>([](int v){return v > state_min && v < state_max;})),
	config(c), log(kernel.get_log())
{
	log<<"init state driver building: preparing video..."<<std::endl;
	prepare_video(kernel);

	log<<"preparing shared resources..."<<std::endl;
	s_resources.reset(
		new shared_resources(
			kernel.get_video_resource_manager(),
			log,
			kernel.get_controller_chrono(),
			kernel.get_arg_manager()
			));

	log<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	log<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input());

	log<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(dfw::kernel& kernel)
{
	auto& screen=kernel.get_screen();

	int wf=config.int_from_path("config:video:window_w_logical"),
		hf=config.int_from_path("config:video:window_h_logical"),
		wl=config.int_from_path("config:video:window_w_px"),
		hl=config.int_from_path("config:video:window_h_px");

	screen.init(wf, hf);
	screen.set_logical_size(wl, hl);
}

void state_driver::register_controllers(dfw::kernel& /*kernel*/)
{
	c_test.reset(new controller_test(*s_resources));
	c_console.reset(new controller_console(*s_resources));
	c_fps.reset(new controller_fps_test(*s_resources));

	register_controller(t_states::state_main, *c_test);
	register_controller(t_states::state_console, *c_console);
	register_controller(t_states::state_fps_test, *c_fps);
}

void state_driver::prepare_state(int next, int /*current*/)
{
	switch(next)
	{
		case t_states::state_main:
		case t_states::state_console:
		case t_states::state_fps_test:    
		break;
	}
}

void state_driver::common_pre_loop_input(dfw::input& input, float /*delta*/)
{
	if(input().is_event_joystick_connected())
	{
		log<<"New joystick detected..."<<std::endl;
		virtualize_input(input);
	}
}

void state_driver::common_loop_input(dfw::input& input, float /*delta*/)
{
#ifdef WDEBUG_CODE
	if(input.is_input_down(input_app::reload_debug_config))
	{
		log<<"reloading debug configuration"<<std::endl;
		s_resources->reload_debug_config();
	}
#endif
}

void state_driver::common_pre_loop_step(float /*delta*/)
{

}

void state_driver::common_loop_step(float /*delta*/)
{

}

void state_driver::virtualize_input(dfw::input& input)
{
	log<<"trying to virtualize "<<input().get_joysticks_size()<<" controllers..."<<std::endl;

	for(size_t i=0; i < input().get_joysticks_size(); ++i)
	{
		input().virtualize_joystick_hats(i);
		input().virtualize_joystick_axis(i, 15000);
		log<<"Joystick virtualized "<<i<<std::endl;
	}
}
