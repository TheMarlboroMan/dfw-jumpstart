#pragma once

//local.
#include "config.h"

//Specific app_config
#include "../app/shared_resources.h"
#include "../controller/signals.h"

//Framework
#include <dfw/state_driver_interface.h>

//Other
#include <ldtools/time_definitions.h>
#include <appenv/env.h>

//std
#include <memory>

namespace dfwimpl
{

class state_driver:
	public dfw::state_driver_interface {

	public:
	                                state_driver(dfwimpl::config& config, lm::logger&, const appenv::env&, int);

	virtual void					common_pre_loop_input(dfw::input& input, ldtools::tdelta);
	virtual void					common_pre_loop_step(ldtools::tdelta);
	virtual void					common_loop_input(dfw::input& input, ldtools::tdelta);
	virtual void					common_loop_step(ldtools::tdelta);
	virtual void					prepare_state(int, int);
	virtual ldtools::tdelta         get_max_timestep() const {return 0.03;}

	void						receive_signal(dfw::kernel&, const dfw::broadcast_signal&);
	void						startup_set_state(int v) {states.set(v);}
	void                        init(dfw::kernel&);

	private:

	void                        prepare_video(dfw::kernel&, dfw::window_info, bool);
	void						prepare_audio(dfw::kernel&);
	void						prepare_input(dfw::kernel&);
	void						prepare_resources(dfw::kernel&);
	void 						virtualize_input(dfw::input& input, int);

	bool                        validate_state(int);
	dfw::window_info            get_video_init_data() const;
	dfw::audio_info             get_audio_init_data() const;
	void                        load_resources(dfw::kernel&);
	void                        load_fonts(ldtools::ttf_manager&);
	void                        register_controllers(dfw::kernel&);
	void                        ready_resources(dfw::kernel&);
	std::vector<dfw::input_pair>    get_input_pairs() const;
	void                        start_app(const tools::arg_manager&, dfw::input&);
	int                         get_input_axis_threshold() const;
	bool                        get_fullscreen() const;
	std::string                 build_resource_path(const std::string, const std::string) const;

	void						setup_signal_receiver(dfw::kernel&);

	//references
	dfwimpl::config&			config;
	lm::logger&					log; //Kernel's log.
	const appenv::env&          env;

	controller::app_receiver	receiver;

	std::unique_ptr<app::shared_resources>		s_resources;

	typedef std::unique_ptr<dfw::controller_interface> ptr_controller;
	ptr_controller		c_menu;
	ptr_controller		c_test_2d;
	ptr_controller	    c_test_2d_text;
	ptr_controller		c_test_poly;
	ptr_controller		c_console;
	ptr_controller		c_fps;
	ptr_controller		c_step;
};

}
