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

extern tools::log LOG;

state_driver::state_driver(dfw::kernel& kernel, app::app_config& c)
	:state_driver_interface(t_states::state_menu),
	config(c), log(kernel.get_log()), receiver(get_signal_dispatcher()) {

	log<<"setting state check function..."<<std::endl;
	states.set_function([](int v){
		return v > state_min && v < state_max;
	});

	log<<"init state driver building: preparing video..."<<std::endl;
	prepare_video(kernel);

	log<<"preparing audio..."<<std::endl;
	prepare_audio(kernel);

	log<<"preparing input..."<<std::endl;
	prepare_input(kernel);

	log<<"preparing resources..."<<std::endl;
	prepare_resources(kernel);

	log<<"preparing resource injection class..."<<std::endl;
	s_resources.reset(
		new shared_resources(
			kernel.get_audio(),
			kernel.get_audio_resource_manager(),
			kernel.get_video_resource_manager(),
			log,
			kernel.get_controller_chrono(),
			kernel.get_arg_manager()
			));

	log<<"registering signal receiver..."<<std::endl;
	setup_signal_receiver(kernel);

	log<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	log<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input());
//	kernel.set_delta_step(0.01f);

	log<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(dfw::kernel& kernel) {

	kernel.init_video_system({
		config.int_from_path("config:video:window_w_px"),
		config.int_from_path("config:video:window_h_px"),
		config.int_from_path("config:video:window_w_logical"),
		config.int_from_path("config:video:window_h_logical"),
		config.string_from_path("config:video:window_title"),
		config.bool_from_path("config:video:window_show_cursor"),
		config.get_screen_vsync()
	});

	auto& screen=kernel.get_screen();
	screen.set_fullscreen(config.bool_from_path("config:video:fullscreen"));
}

void state_driver::prepare_audio(dfw::kernel& kernel) {

	kernel.init_audio_system({
		config.get_audio_ratio(),
		config.get_audio_out(),
		config.get_audio_buffers(),
		config.get_audio_channels(),
		config.get_audio_volume(),
		config.get_music_volume()
	});
}

void state_driver::prepare_input(dfw::kernel& kernel) {

	using namespace dfw;

	std::vector<input_pair> pairs{
		{{input_description::types::keyboard, SDL_SCANCODE_ESCAPE, 0}, input_app::escape},
		{input_description_from_config_token(config.token_from_path("config:input:left")), input_app::left},
		{input_description_from_config_token(config.token_from_path("config:input:right")), input_app::right},
		{input_description_from_config_token(config.token_from_path("config:input:up")), input_app::up},
		{input_description_from_config_token(config.token_from_path("config:input:down")), input_app::down},
		{input_description_from_config_token(config.token_from_path("config:input:activate")), input_app::activate},
		{input_description_from_config_token(config.token_from_path("config:input:console_newline")), input_app::console_newline},
		{input_description_from_config_token(config.token_from_path("config:input:console_backspace")), input_app::console_backspace}
#ifdef WDEBUG_CODE
		,{input_description_from_config_token(config.token_from_path("config:input:reload_debug_config")), input_app::reload_debug_config}
#endif
	};

	kernel.init_input_system(pairs);
}

void state_driver::prepare_resources(dfw::kernel& kernel) {

	dfw::resource_loader r_loader(kernel.get_video_resource_manager(), kernel.get_audio_resource_manager());

	r_loader.generate_textures(tools::explode_lines_from_file(std::string("data/resources/textures.txt")));
	r_loader.generate_sounds(tools::explode_lines_from_file(std::string("data/resources/audio.txt")));
	r_loader.generate_music(tools::explode_lines_from_file(std::string("data/resources/music.txt")));
}

void state_driver::register_controllers(dfw::kernel& /*kernel*/) {

	auto& dispatcher=get_signal_dispatcher();

	c_menu.reset(new controller_menu(*s_resources, dispatcher, config));
	c_test_2d.reset(new controller_test_2d(*s_resources, dispatcher));
	c_test_2d_text.reset(new controller_test_2d_text(*s_resources, dispatcher));
	c_test_poly.reset(new controller_test_poly(*s_resources, dispatcher));
	c_console.reset(new controller_console(*s_resources));
	c_fps.reset(new controller_fps_test(*s_resources));
	c_step.reset(new controller_step(*s_resources));

	register_controller(t_states::state_menu, *c_menu);
	register_controller(t_states::state_test_2d, *c_test_2d);
	register_controller(t_states::state_test_2d_text, *c_test_2d_text);
	register_controller(t_states::state_test_poly, *c_test_poly);
	register_controller(t_states::state_console, *c_console);
	register_controller(t_states::state_fps_test, *c_fps);
	register_controller(t_states::state_step, *c_step);
}

void state_driver::prepare_state(int next, int current) {

	switch(next) {
		case t_states::state_menu:
			c_menu->set_continue_state(current);
		break;
		case t_states::state_test_2d:
		case t_states::state_test_poly:
		case t_states::state_test_2d_text:
		case t_states::state_console:
		case t_states::state_fps_test:
		case t_states::state_step:
		break;
	}
}

void state_driver::common_pre_loop_input(dfw::input& input, float /*delta*/) {

	if(input().is_event_joystick_connected()) {
		log<<"New joystick detected..."<<std::endl;
		virtualize_input(input);
	}
}

void state_driver::common_loop_input(dfw::input& input, float /*delta*/) {
#ifdef WDEBUG_CODE
	if(input.is_input_down(input_app::reload_debug_config)) {
		log<<"reloading debug configuration"<<std::endl;
		s_resources->reload_debug_config();
	}
#endif
}

void state_driver::common_pre_loop_step(float /*delta*/) {

}

void state_driver::common_loop_step(float /*delta*/) {

}

void state_driver::virtualize_input(dfw::input& input) {
	log<<"trying to virtualize "<<input().get_joysticks_size()<<" controllers..."<<std::endl;

	for(size_t i=0; i < input().get_joysticks_size(); ++i) {
		input().virtualize_joystick_hats(i);
		input().virtualize_joystick_axis(i, 15000);
		log<<"Joystick virtualized "<<i<<std::endl;
	}
}

void state_driver::setup_signal_receiver(dfw::kernel& kernel) {
	receiver.f=[this, &kernel](const dfw::broadcast_signal& s) {receive_signal(kernel, s);};
}

void state_driver::receive_signal(dfw::kernel& kernel, const dfw::broadcast_signal& s) {

	switch(s.get_type()) {
		case t_signal_video_size: {
			const std::string& val=static_cast<const signal_video_size&>(s).value;
			if(val=="fullscreen") {
				kernel.get_screen().set_size(config.int_from_path("config:video:window_h_logical"), config.int_from_path("config:video:window_h_logical"));
				kernel.get_screen().set_fullscreen(true);
			}
			else {
				const auto& parts=tools::explode(val, 'x');
				kernel.get_screen().set_fullscreen(false);
				kernel.get_screen().set_size(std::atoi(parts[0].c_str()), std::atoi(parts[1].c_str()));
			}
		}
		break;
		case t_signal_video_vsync:
			ldv::set_vsync(static_cast<const signal_video_vsync&>(s).value);
		break;
		case t_signal_audio_volume:
			kernel.get_audio()().set_main_sound_volume(static_cast<const signal_audio_volume&>(s).value);
		break;
		case t_signal_music_volume:
			kernel.get_audio()().set_main_music_volume(static_cast<const signal_music_volume&>(s).value);
		break;
		case t_signal_save_configuration:
			config.set_audio_volume(kernel.get_audio()().get_main_sound_volume());
			config.set_music_volume(kernel.get_audio()().get_main_music_volume());
			config.set_screen_vsync(ldv::get_vsync());
			config.set("config:video:fullscreen", kernel.get_screen().is_fullscreen());
			config.set<int>("config:video:window_w_px", kernel.get_screen().get_w());
			config.set<int>("config:video:window_h_px", kernel.get_screen().get_h());
			config.save();
		break;
		case t_signal_save_controls:
			config.set<tools::dnot_token>("config:input:up", 	config_token_from_input_description(kernel.get_input().locate_description(input_app::up)));
			config.set<tools::dnot_token>("config:input:down",  	config_token_from_input_description(kernel.get_input().locate_description(input_app::down)));
			config.set<tools::dnot_token>("config:input:left", 	config_token_from_input_description(kernel.get_input().locate_description(input_app::left)));
			config.set<tools::dnot_token>("config:input:right", 	config_token_from_input_description(kernel.get_input().locate_description(input_app::right)));
			config.set<tools::dnot_token>("config:input:activate", 	config_token_from_input_description(kernel.get_input().locate_description(input_app::activate)));
			config.save();
		break;
	}
}
