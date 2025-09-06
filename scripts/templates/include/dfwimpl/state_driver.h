#pragma once

//Dependencies.
#include "config.h"
#include <dfw/state_driver_interface.h>
#include <ldtools/time_definitions.h>
#include <ldtools/ttf_manager.h>
#include <appenv/env.h>
#include <memory>

//Specific app_config
//#include "app/placeholder.h"

namespace dfwimpl {

using ldtools::tdelta;

class state_driver:
	public dfw::state_driver_interface {

	public:
	                            state_driver(dfwimpl::config& config, lm::logger&, const appenv::env&, int);

	virtual void                common_pre_loop_input(dfw::input&, tdelta);
	virtual void                common_pre_loop_step(tdelta);
	virtual void                common_loop_input(dfw::input& input, tdelta);
	virtual void                common_loop_step(tdelta);
	virtual void                prepare_state(int, int);
	virtual tdelta              get_max_timestep() const {return 0.03;}
	void                        init(dfw::kernel&);

	private:

	//!The last arg indicates whether the cursor shall be displayed.
	void                        prepare_video(dfw::kernel&, dfw::window_info, bool);
	void                        prepare_audio(dfw::kernel&);
	void                        prepare_input(dfw::kernel&);
	void                        prepare_resources(dfw::kernel&);
	void                        virtualize_input(dfw::input& input, int);

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

	//references
	dfwimpl::config&            config;
	lm::logger&                 log;
	const appenv::env&          env;

	//state driver data.
	
	//controllers.
	typedef std::unique_ptr<dfw::controller_interface> ptr_controller;
	//[new-controller-property-mark]
};

}
