#pragma once

#include "config.h"

#include <dfw/state_driver_interface.h>

//Other
#include <ldtools/time_definitions.h>

#include <memory>

//Controllers.
//[new-controller-header-mark]

//Specific app_config
//#include "../app/placeholder.h"

namespace dfwimpl {

class state_driver:
	public dfw::state_driver_interface {

	public:
	                            state_driver(dfw::kernel& kernel, dfwimpl::config& config);

	void                        init(dfw::kernel&);
	virtual void                common_pre_loop_input(dfw::input&, ldtools::tdelta);
	virtual void                common_pre_loop_step(ldtools::tdelta);
	virtual void                common_loop_input(dfw::input& input, ldtools::tdelta);
	virtual void                common_loop_step(ldtools::tdelta);
	virtual void                prepare_state(int, int);
	virtual ldtools::tdelta     get_max_timestep() const {return 0.03f;}

	private:

	void                        prepare_video(dfw::kernel&);
	void                        prepare_audio(dfw::kernel&);
	void                        prepare_input(dfw::kernel&);
	void                        prepare_resources(dfw::kernel&);
	void                        register_controllers(dfw::kernel&);
	void                        virtualize_input(dfw::input& input);

	//references
	dfwimpl::config&            config;
	lm::logger&                 log;

	typedef std::unique_ptr<dfw::controller_interface> ptr_controller;
	//[new-controller-property-mark]
};

}
