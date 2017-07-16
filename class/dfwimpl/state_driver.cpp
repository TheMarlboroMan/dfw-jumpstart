#include "state_driver.h"

//std
#include <algorithm>

//tools
#include <class/dnot_parser.h>
#include <source/string_utils.h>

using namespace app;

extern ldt::log LOG;

state_driver::state_driver(dfw::kernel& kernel, app::app_config& c)
	:state_driver_interface(t_states::state_game, std::function<bool(int)>([](int v){return v > state_min && v < state_max;})),
	config(c), log(kernel.get_log())
{
	prepare_video(kernel);
	register_fonts();
	register_controllers(kernel);
	virtualize_input(kernel.get_input());
	
	log<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(dfw::kernel& kernel)
{
	log<<"preparing app video configuration..."<<std::endl;
	auto& screen=kernel.get_screen();

	int wf=config.int_from_path("config:video:window_w_logical"),
		hf=config.int_from_path("config:video:window_h_logical"),
		wl=config.int_from_path("config:video:window_w_px"),
		hl=config.int_from_path("config:video:window_h_px");

	screen.init(wf, hf);
	screen.set_logical_size(wl, hl);
}

void state_driver::register_controllers(dfw::kernel& kernel)
{
	log<<"registering controllers..."<<std::endl;
	c_test.reset(new controller_test(log, kernel.get_video_resource_manager(), fonts));

	register_controller(t_states::state_game, *c_test);
}

void state_driver::prepare_state(int next, int /*current*/)
{
	switch(next)
	{
		case t_states::state_game: 
		break;
	}
}

void state_driver::common_input(dfw::input& input, float /*delta*/)
{
	if(input().is_event_joystick_connected())
	{
		log<<"New joystick detected..."<<std::endl;
		virtualize_input(input);
	}
}

void state_driver::common_step(float /*delta*/)
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

void state_driver::register_fonts()
{	
	log<<"registering fonts..."<<std::endl;

	using namespace tools;

	auto v=explode_lines_from_file("data/resources/fonts.txt");
	for(const auto& l : v)
	{
		auto p=explode(l, '\t');
		if(p.size()!=3)
		{
			throw std::runtime_error("unable to parse font file: 3 parameters expected");
		}
		else
		{
			fonts.insert(p[0], std::atoi( p[1].c_str() ), p[2] );
		}
	}	
}
