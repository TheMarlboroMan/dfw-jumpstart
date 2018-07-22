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

	log<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	log<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input());

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

	//c_placeholder.reset(new controller_placeholder());
	//register_controller(t_states::state_placeholder, *c_placeholder);
}

void state_driver::prepare_state(int next, int current) {

/*
	switch(next) {
		case t_states::state_placeholder:
		break;
		default:
		break;
	}
*/
}

void state_driver::common_pre_loop_input(dfw::input& input, float /*delta*/) {

	if(input().is_event_joystick_connected()) {
		log<<"New joystick detected..."<<std::endl;
		virtualize_input(input);
	}
}

void state_driver::common_loop_input(dfw::input& input, float /*delta*/) {

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
