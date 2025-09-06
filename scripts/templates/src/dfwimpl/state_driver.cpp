//Generic dependencies.
#include "dfwimpl/state_driver.h"
#include "app/input.h"
#include "controller/states.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <lm/log.h>

//Controllers
//[new-controller-header-mark]

//libraries and application specifics
//#include <tools/string_utils.h>

using namespace dfwimpl;

state_driver::state_driver(
	dfwimpl::config& _config,
	lm::logger& _logger,
	const appenv::env& _env,
	int _initial_state
)
	:state_driver_interface{_initial_state},
	config{_config}, 
	log{_logger},
	env{_env}
{}

/**
 * We will have three blocks here. First are the most common things that we
 * should not fiddle too much with. Then the "config" / "behaviour" parts and 
 * finally the "application-specific" parts. These large comment banners 
 * separate each block.
 */

/**
 * These are sort of generic methods for most applications that get their 
 * data from elsewhere. We don't need to change much here.
 */

void state_driver::init(
	dfw::kernel& kernel
) {
	//Do some black magic.
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

	lm::log(log).info()<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	lm::log(log).info()<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input(), get_input_axis_threshold());

	lm::log(log).info()<<"state driver fully constructed"<<std::endl;

	start_app(kernel.get_arg_manager(), kernel.get_input());
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

void state_driver::prepare_input(dfw::kernel& kernel) {

	auto pairs=get_input_pairs();
	kernel.init_input_system(pairs);
}

void state_driver::prepare_resources(
	dfw::kernel& _kernel
) {

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

void state_driver::common_loop_input(dfw::input& /*input*/, ldtools::tdelta /*delta*/) {

}

void state_driver::common_pre_loop_step(ldtools::tdelta /*delta*/) {

}

void state_driver::common_loop_step(ldtools::tdelta /*delta*/) {

}

/**
 * From here on these are "config" and "behaviour" like methods. Everything 
 * here could and should be changed to adapt to the application specific needs 
 * even if the structure can still be reused. Not everything here is a config, 
 * for example, load_resources can choose what kind of stuff to load.
 */

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
		{{input_description::types::keyboard, SDL_SCANCODE_ESCAPE, 0}, app::escape}
	};

	auto add=[&](std::string _token, int _input_type) {

		for(const auto desc : input_description_from_config_token(config.token_from_path(_token))) {

			lm::log(log).info()<<"setting input for "<<_input_type<<" as code "<<desc.code<<std::endl;
			pairs.push_back({desc, _input_type});
		}
	};

	std::vector<std::tuple<std::string, int>> controls={
		{"input:left", app::left},
		{"input:right", app::right},
		{"input:up", app::up},
		{"input:down", app::down}
	};

	for(const auto& pair : controls) {

		add(std::get<0>(pair), std::get<1>(pair));
	}

	return pairs;
}

void state_driver::load_resources(
	dfw::kernel& /*_kernel*/
) {

	/*
	dfw::resource_loader r_loader(
		_kernel.get_video_resource_manager(), 
		_kernel.get_audio_resource_manager(), 
		env.build_app_path("")
	);
	*/

	//Textures.
	//r_loader.generate_textures(tools::explode_lines_from_file(build_resource_path("lists", "textures.txt")));

	//Ssurfaces that may need to be loaded, for later manipulation into composite backgrounds.
	//r_loader.generate_surfaces(tools::explode_lines_from_file(build_resource_path("lists", "surfaces.txt")));
	
	//Sounds.
	//r_loader.generate_sounds(tools::explode_lines_from_file(build_resource_path("lists", "sounds.txt")));
	
	//Music, unless it will be loaded and unloaded dynamically.
	//r_loader.generate_music(tools::explode_lines_from_file(build_resource_path("lists", "music.txt")));
}

void state_driver::ready_resources(
	dfw::kernel& _kernel
) {

/*
	//The service provider is a resource.
	service_provider.reset(
		new app::service_provider{env, config, log, _kernel}
	);

	std::string window_title="Some title";
	_kernel.get_screen().set_title(window_title);

	//Ready ttf fonts...
	auto &ttf_manager=service_provider->get_ttf_manager();
	load_fonts(ttf_manager);
*/
}

void state_driver::load_fonts(
	ldtools::ttf_manager& _ttf_manager
) {

	std::ifstream font_stream{build_resource_path("lists", "fonts.txt")};
	std::string font_id, font_path;
	int font_size;

	while(true) {

		font_stream>>font_id>>font_size>>font_path;

		if(font_stream.eof()) {

			break;
		}

		_ttf_manager.insert(font_id, font_size, env.build_app_path(font_path));
	}
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

	return _v > controller::state_min 
		&& _v < controller::state_max;
}

std::string state_driver::build_resource_path(
	const std::string _type,
	const std::string _str
) const {

	std::stringstream ss;
	ss<<"resources/"<<_type<<"/"<<_str;
	return env.build_app_path(ss.str());
};

/**
 * And here are the "application-specific" pieces. The structure of 
 * register_controllers is likely to be the same but both prepare_state 
 * and start_app can be trashed and started anew.
 */

void state_driver::register_controllers(
	dfw::kernel& 
) {

	auto reg=[this](ptr_controller& _ptr, int _i, dfw::controller_interface * _ci) {

		_ptr.reset(_ci);
		register_controller(_i, *_ptr);
	};

	//[new-controller-mark]
}

void state_driver::prepare_state(
	int /*_next*/, 
	int /*_current*/
) {

/*
	switch(next) {
		case t_states::state_placeholder:
		break;
		default:
		break;
	}
*/
}

void state_driver::start_app(
	const tools::arg_manager&,
	dfw::input&
) {

}

