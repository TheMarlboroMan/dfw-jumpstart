#ifndef APP_STATE_DRIVER_H
#define APP_STATE_DRIVER_H

//std
#include <memory>

//Framework
#include <class/state_driver_interface.h>

//local.
#include "app_config.h"

//Controllers.
#include "../controllers/states.h"
#include "../controllers/menu.h"
#include "../controllers/test_2d.h"
#include "../controllers/test_2d_text.h"
#include "../controllers/fps_test.h"
#include "../controllers/console.h"
#include "../controllers/test_poly.h"
#include "../controllers/step.h"
#include "../controllers/signals.h"

//Specific app_config
#include "../app/shared_resources.h"

namespace app
{

class state_driver:
	public dfw::state_driver_interface
{
	public:

							state_driver(dfw::kernel& kernel, app::app_config& config);
	
	virtual void					common_pre_loop_input(dfw::input& input, float delta);
	virtual void					common_pre_loop_step(float delta);
	virtual void					common_loop_input(dfw::input& input, float delta);
	virtual void					common_loop_step(float delta);
	virtual void					prepare_state(int, int);
	virtual float					get_max_timestep() const {return 0.03f;}

	void						receive_signal(dfw::kernel&, const dfw::broadcast_signal&);

	private:

	void						prepare_video(dfw::kernel&);
	void						register_controllers(dfw::kernel&);
	void 						virtualize_input(dfw::input& input);
	void						setup_signal_receiver(dfw::kernel&);

	//references
	app::app_config&				config;
	ldt::log&					log;

	app_receiver					receiver;

	std::unique_ptr<shared_resources>		s_resources;
	std::unique_ptr<controller_menu>		c_menu;
	std::unique_ptr<controller_test_2d>		c_test_2d;
	std::unique_ptr<controller_test_2d_text>	c_test_2d_text;
	std::unique_ptr<controller_test_poly>		c_test_poly;
	std::unique_ptr<controller_console>		c_console;
	std::unique_ptr<controller_fps_test>		c_fps;
	std::unique_ptr<controller_step>		c_step;
};

}
#endif
