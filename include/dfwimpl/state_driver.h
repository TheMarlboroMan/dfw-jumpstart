#pragma once

//local.
#include "config.h"

//Controllers.
#include "../controller/states.h"
#include "../controller/menu.h"
#include "../controller/test_2d.h"
#include "../controller/test_2d_text.h"
#include "../controller/fps_test.h"
#include "../controller/console.h"
#include "../controller/test_poly.h"
#include "../controller/step.h"
#include "../controller/signals.h"

//Specific app_config
#include "../app/shared_resources.h"

//Framework
#include <dfw/state_driver_interface.h>

//Other
#include <ldtools/time_definitions.h>

//std
#include <memory>

namespace dfwimpl
{

class state_driver:
	public dfw::state_driver_interface {

	public:
									state_driver(dfw::kernel& kernel, dfwimpl::config& config);

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

	void						prepare_video(dfw::kernel&);
	void						prepare_audio(dfw::kernel&);
	void						prepare_input(dfw::kernel&);
	void						prepare_resources(dfw::kernel&);
	void						register_controllers(dfw::kernel&);
	void 						virtualize_input(dfw::input& input);
	void						setup_signal_receiver(dfw::kernel&);

	//references
	dfwimpl::config&			config;
	lm::logger&					log; //Kernel's log.

	controller::app_receiver	receiver;

	std::unique_ptr<app::shared_resources>		s_resources;
	std::unique_ptr<controller::menu>		c_menu;
	std::unique_ptr<controller::test_2d>		c_test_2d;
	std::unique_ptr<controller::test_2d_text>	c_test_2d_text;
	std::unique_ptr<controller::test_poly>		c_test_poly;
	std::unique_ptr<controller::console>		c_console;
	std::unique_ptr<controller::fps_test>		c_fps;
	std::unique_ptr<controller::step>		c_step;
};

}
