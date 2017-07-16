#include <libDan2.h>
#include <class/kernel.h>
#include "class/dfwimpl/kernel_config.h"
#include "class/dfwimpl/app_config.h"
#include "class/dfwimpl/state_driver.h"

//Global log. Bad practice, but useful.
ldt::log LOG;

int main(int argc, char ** argv)
{
	using namespace app;

	//Init global log.
	LOG.init("logs/global.log");
	LOG.activate();

	//Init libdansdl2 log.
	ldt::log_lsdl::init("logs/libdansdl2.log");

	//Argument controller.
	tools::arg_manager CARG(argc, argv);

	//Init application log.
	ldt::log log_app("logs/app.log");
	log_app<<"starting main process..."<<std::endl;

	//Init...
	try
	{
		log_app<<"init app config..."<<std::endl;
		app_config config;

		//Load kernel, load SDL.
		log_app<<"init kernel config..."<<std::endl;
		kernel_config kconfig(config);

		log_app<<"init sdl2..."<<std::endl;	
		if(!ldt::sdl_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK))
		{
			throw std::runtime_error("unable to init sdl2");
		}
		
		log_app<<"creating kernel..."<<std::endl;
		dfw::kernel kernel(log_app, CARG);

		log_app<<"init kernel..."<<std::endl;
		kernel.init(kconfig, config);

		log_app<<"create state driver..."<<std::endl;
		state_driver APP(kernel, config);

		log_app<<"init state driver..."<<std::endl;
		APP.init(kernel);

		log_app<<"finish main proccess"<<std::endl;
	}
	catch(std::exception& e)
	{
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
