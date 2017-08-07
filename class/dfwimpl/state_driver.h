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
#include "../controllers/test.h"
#include "../controllers/console.h"

//Specific app_config
#include "../app/shared_resources.h"

namespace app
{

class state_driver:
	public dfw::state_driver_interface
{
	public:

							state_driver(dfw::kernel& kernel, app::app_config& config);
	
	virtual void					common_input(dfw::input& input, float delta);
	virtual void					common_step(float delta);
	virtual void					prepare_state(int, int);
	virtual float					get_max_timestep() const {return 0.03f;}

	private:

	void						prepare_video(dfw::kernel&);
	void						register_controllers(dfw::kernel&);
	void 						virtualize_input(dfw::input& input);

	app::app_config&				config;
	ldt::log&					log;

	std::unique_ptr<shared_resources>		s_resources;
	std::unique_ptr<controller_test>		c_test;
	std::unique_ptr<controller_console>		c_console;
};

}
#endif
