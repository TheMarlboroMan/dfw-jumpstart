#include <libDan2.h>
#include <class/kernel.h>
#include "class/dfwimpl/app_config.h"
#include "class/dfwimpl/state_driver.h"
#include <src/log.h>

//Global log. Bad practice, but useful.
tools::log LOG;

int main(int argc, char ** argv)
{
	using namespace app;

	//Init global log.
	LOG.init("logs/global.log");
	LOG.activate();

	//Init libdansdl2 log.
	ldt::log_lsdl::init("logs/libdansdl2.log");

	//Argument controller.
	tools::arg_manager carg(argc, argv);

	//Init application log.
	tools::log log_app("logs/app.log");
	log_app<<"starting main process..."<<std::endl;

	//Init...
	try {
		log_app<<"init sdl2..."<<std::endl;
		if(!ldt::sdl_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK)) {
			throw std::runtime_error("unable to init sdl2");
		}
		
		log_app<<"creating kernel..."<<std::endl;
		dfw::kernel kernel(log_app, carg);

		log_app<<"init app config..."<<std::endl;
		app_config config;

		log_app<<"create state driver..."<<std::endl;
		state_driver sd(kernel, config);

		//Setting the state according to the command line...
		if(carg.exists("-s") && carg.arg_follows("-s")) {
			sd.startup_set_state(std::atoi(carg.get_following("-s").c_str()));
		} 

		log_app<<"init state driver..."<<std::endl;
		sd.init(kernel);

		log_app<<"finish main proccess"<<std::endl;
	}
	catch(std::exception& e) {
		std::cout<<"Interrupting due to exception: "<<e.what()<<std::endl;
		log_app<<"an error happened "<<e.what()<<std::endl;
		log_app<<"stopping sdl2..."<<std::endl;
		ldt::sdl_shutdown();
		ldt::log_lsdl::end();

		return 1;
	}

	log_app<<"stopping sdl2..."<<std::endl;
	ldt::sdl_shutdown();
	ldt::log_lsdl::end();
	return 0;
}
