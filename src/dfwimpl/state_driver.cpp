#include "../../include/dfwimpl/state_driver.h"
#include "../../include/input/input.h"

//Controllers.
#include "../../include/controller/states.h"
#include "../../include/controller/menu.h"
#include "../../include/controller/test_2d.h"
#include "../../include/controller/test_2d_text.h"
#include "../../include/controller/fps_test.h"
#include "../../include/controller/console.h"
#include "../../include/controller/test_poly.h"
#include "../../include/controller/step.h"
#include "../../include/controller/signals.h"

//tools
#include <tools/string_utils.h>
#include <lm/log.h>

//std
#include <algorithm>

using namespace dfwimpl;

state_driver::state_driver(
	dfwimpl::config& _config,
	lm::logger& _logger,
	const appenv::env& _env,
	int _initial_state
)
	:state_driver_interface(_initial_state),
	config(_config),
	log(_logger),
	env{_env},
	receiver(get_signal_dispatcher()) 
{ }

void state_driver::init(
	dfw::kernel& kernel
) {

	lm::log(log).info()<<"setting state check function..."<<std::endl;
	using std::placeholders::_1;
	std::function<bool(int)> vfunc=std::bind(&state_driver::validate_state, this, _1);
	states.set_function(vfunc);

	lm::log(log).info()<<"init state driver building: preparing video..."<<std::endl;
	prepare_video(kernel, get_video_init_data(), get_fullscreen());

	lm::log(log).info()<<"preparing audio..."<<std::endl;
	prepare_audio(kernel);

	lm::log(log).info()<<"preparing input..."<<std::endl;
	prepare_input(kernel);

	lm::log(log).info()<<"preparing resources..."<<std::endl;
	prepare_resources(kernel);

	lm::log(log).info()<<"preparing resource injection class..."<<std::endl;
	s_resources.reset(
		new app::shared_resources(
			kernel.get_audio(),
			kernel.get_audio_resource_manager(),
			kernel.get_video_resource_manager(),
			log,
			kernel.get_controller_chrono(),
			kernel.get_arg_manager()
			));

	lm::log(log).info()<<"registering signal receiver..."<<std::endl;
	setup_signal_receiver(kernel);

	lm::log(log).info()<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	lm::log(log).info()<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input(), get_input_axis_threshold());
//	kernel.set_delta_step(0.01);

	lm::log(log).info()<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(
	dfw::kernel& _kernel,
	dfw::window_info _window_info,
	bool _fullscreen
) {

	_kernel.init_video_system(_window_info);
	_kernel.get_screen().set_fullscreen(_fullscreen);
}

void state_driver::prepare_audio(dfw::kernel& kernel) {

	kernel.init_audio_system(get_audio_init_data());
}

void state_driver::prepare_input(dfw::kernel& _kernel) {

	auto pairs=get_input_pairs();
	_kernel.init_input_system(pairs);
}

void state_driver::prepare_resources(dfw::kernel& _kernel) {

	load_resources(_kernel);
	ready_resources(_kernel);
}

void state_driver::virtualize_input(
	dfw::input& input,
	int _threshold
) {

	lm::log(log).info()<<"trying to virtualize "<<input().get_joysticks_size()<<" controllers..."<<std::endl;

	for(size_t i=0; i < input().get_joysticks_size(); ++i) {

		input().virtualize_joystick_hats(i);
		input().virtualize_joystick_axis(i, _threshold);
		lm::log(log).info()<<"Joystick virtualized "<<i<<std::endl;
	}
}

void state_driver::common_pre_loop_input(
	dfw::input& input, 
	ldtools::tdelta /*delta*/
) {

	if(input().is_event_joystick_connected()) {

		lm::log(log).info()<<"New joystick detected..."<<std::endl;
		virtualize_input(input, get_input_axis_threshold());
	}
}

#ifdef WDEBUG_CODE
void state_driver::common_loop_input(
	dfw::input& input, 
	ldtools::tdelta
) {

	if(input.is_input_down(input::reload_debug_config)) {
		lm::log(log).info()<<"reloading debug configuration"<<std::endl;
		s_resources->reload_debug_config();
	}
}
#else
void state_driver::common_loop_input(
	dfw::input& /*input*/, 
	ldtools::tdelta
) {

}
#endif

void state_driver::common_pre_loop_step(ldtools::tdelta /*delta*/) {

}

void state_driver::common_loop_step(ldtools::tdelta /*delta*/) {

}

dfw::window_info state_driver::get_video_init_data() const {

	return {
		config.int_from_path("video:window_w_px"),
		config.int_from_path("video:window_h_px"),
		config.int_from_path("video:window_w_logical"),
		config.int_from_path("video:window_h_logical"),
		config.string_from_path("video:window_title"),
		config.bool_from_path("video:window_show_cursor"),
		config.get_screen_vsync()
	};
}

dfw::audio_info state_driver::get_audio_init_data() const {

	return {
		config.get_audio_ratio(),
		config.get_audio_out(),
		config.get_audio_buffers(),
		config.get_audio_channels(),
		config.get_audio_volume(),
		config.get_music_volume()
	};
}

std::vector<dfw::input_pair> state_driver::get_input_pairs() const {

	using namespace dfw;

	std::vector<input_pair> pairs{
		{{input_description::types::keyboard, SDL_SCANCODE_ESCAPE, 0}, input::escape}
	};

	auto add=[&](std::string _token, int _input_type) {

		for(const auto desc : input_description_from_config_token(config.token_from_path(_token))) {

			pairs.push_back({desc, _input_type});
		}
	};

	add("input:left", input::left);
	add("input:right", input::right);
	add("input:up", input::up);
	add("input:down", input::down);
	add("input:activate", input::activate);
	add("input:console_newline", input::console_newline);
	add("input:console_backspace", input::console_backspace);
#ifdef WDEBUG_CODE
	add("input:reload_debug_config", input::reload_debug_config);
#endif

	return pairs;
}

void state_driver::load_resources(
	dfw::kernel& kernel
) {

	dfw::resource_loader r_loader(
		kernel.get_video_resource_manager(), 
		kernel.get_audio_resource_manager()
	);

	r_loader.generate_textures(tools::explode_lines_from_file(std::string("data/resources/textures.txt")));
	r_loader.generate_sounds(tools::explode_lines_from_file(std::string("data/resources/audio.txt")));
	r_loader.generate_music(tools::explode_lines_from_file(std::string("data/resources/music.txt")));
}

void state_driver::ready_resources(
	dfw::kernel& 
) {

}

void state_driver::load_fonts(
	ldtools::ttf_manager& 
) {

}

int state_driver::get_input_axis_threshold() const {

	return 15000;
}

bool state_driver::get_fullscreen() const {

	return config.bool_from_path("video:fullscreen");
}

bool state_driver::validate_state(
	int _v
) {

	return _v > state_min && _v < state_max;
}

std::string state_driver::build_resource_path(
	const std::string,
	const std::string
) const {

	return "";
};

void state_driver::register_controllers(dfw::kernel& /*kernel*/) {

	auto& dispatcher=get_signal_dispatcher();

	c_menu.reset(new controller::menu(*s_resources, dispatcher, config));
	c_test_2d.reset(new controller::test_2d(*s_resources, dispatcher));
	c_test_2d_text.reset(new controller::test_2d_text(*s_resources, dispatcher));
	c_test_poly.reset(new controller::test_poly(*s_resources, dispatcher));
	c_console.reset(new controller::console(*s_resources));
	c_fps.reset(new controller::fps_test(*s_resources));
	c_step.reset(new controller::step(*s_resources));

	register_controller(t_states::state_menu, *c_menu);
	register_controller(t_states::state_test_2d, *c_test_2d);
	register_controller(t_states::state_test_2d_text, *c_test_2d_text);
	register_controller(t_states::state_test_poly, *c_test_poly);
	register_controller(t_states::state_console, *c_console);
	register_controller(t_states::state_fps_test, *c_fps);
	register_controller(t_states::state_step, *c_step);
}

void state_driver::prepare_state(int next, int current) {

	lm::log(log).debug()<<"state change from "<<current<<" to "<<next<<"..."<<std::endl;

	switch(next) {
		case t_states::state_menu:{

			auto ptr=c_menu.get();
			static_cast<controller::menu *>(ptr)->set_continue_state(current);
		}
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

void state_driver::setup_signal_receiver(dfw::kernel& kernel) {
	receiver.f=[this, &kernel](const dfw::broadcast_signal& s) {receive_signal(kernel, s);};
}

void state_driver::receive_signal(dfw::kernel& kernel, const dfw::broadcast_signal& s) {

	lm::log(log).debug()<<"got a signal"<<std::endl;

	switch(s.get_type()) {
		case controller::t_signal_video_size: {
			const std::string& val=static_cast<const controller::signal_video_size&>(s).value;
			if(val=="fullscreen") {
				kernel.get_screen().set_size(config.int_from_path("video:window_h_logical"), config.int_from_path("video:window_h_logical"));
				kernel.get_screen().set_fullscreen(true);
			}
			else {
				const auto& parts=tools::explode(val, 'x');
				kernel.get_screen().set_fullscreen(false);
				kernel.get_screen().set_size(std::atoi(parts[0].c_str()), std::atoi(parts[1].c_str()));
			}
		}
		break;
		case controller::t_signal_video_vsync:
			ldv::set_vsync(static_cast<const controller::signal_video_vsync&>(s).value);
		break;
		case controller::t_signal_audio_volume:
			kernel.get_audio()().set_main_sound_volume(static_cast<const controller::signal_audio_volume&>(s).value);
		break;
		case controller::t_signal_music_volume:
			kernel.get_audio()().set_main_music_volume(static_cast<const controller::signal_music_volume&>(s).value);
		break;
		case controller::t_signal_save_configuration:
			config.set_audio_volume(kernel.get_audio()().get_main_sound_volume());
			config.set_music_volume(kernel.get_audio()().get_main_music_volume());
			config.set_screen_vsync(ldv::get_vsync());
			config.set("video:fullscreen", kernel.get_screen().is_fullscreen());
			config.set("video:window_w_px", kernel.get_screen().get_w());
			config.set("video:window_h_px", kernel.get_screen().get_h());
			config.save();
		break;
		case controller::t_signal_save_controls:{

			auto set=[&](const std::string _key, int _input) -> void {

				auto tokenobj=config_token_from_input_description(
					kernel.get_input().locate_first_description(_input), 
					config
				);

				rapidjson::Value vec(rapidjson::kArrayType);
				config.add_to_vector(vec, tokenobj);
				config.set_vector(_key, vec);
			};

			set("input:up", input::up);
			set("input:down", input::down);
			set("input:left", input::left);
			set("input:right", input::right);
			set("input:activate", input::activate);

			config.save();
		}
		break;
	}
}

void state_driver::start_app(
	const tools::arg_manager&,
	dfw::input&
) {

}
