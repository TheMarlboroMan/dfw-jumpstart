#include "../include/dfwimpl/config.h"
#include "../include/dfwimpl/state_driver.h"
#include "../include/app/env.h"
#include "../include/controller/states.h"

#include <lm/file_logger.h>
#include <lm/log.h>

#include <dfw/kernel.h>
#include <ldt/sdl_tools.h>
#include <memory>

std::unique_ptr<appenv::env> make_env(lm::logger&);

//Global log. Bad practice, but useful.
lm::file_logger LOG("logs/global.log");

int main(int argc, char ** argv) {

	using namespace app;

	//Init libdansdl2 log.
	ldt::log_lsdl::set_type(ldt::log_lsdl::types::file);
	ldt::log_lsdl::set_filename("logs/libdansdl2.log");

	//Argument controller.
	tools::arg_manager carg(argc, argv);

	//Init application log.
	lm::file_logger log_app("logs/app.log");
	lm::log(log_app).info()<<"starting main process..."<<std::endl;

	auto env=make_env(log_app);

	//Init...
	try {
		lm::log(log_app).info()<<"init sdl2..."<<std::endl;
		if(!ldt::sdl_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK)) {
			throw std::runtime_error("unable to init sdl2");
		}

		lm::log(log_app).info()<<"creating kernel..."<<std::endl;
		dfw::kernel kernel(log_app, carg);

		lm::log(log_app).info()<<"setting up config..."<<std::endl;
		const std::string config_file{env->build_user_path("config.json")};
		dfwimpl::config config(config_file);

		lm::log(log_app).info()<<"create state driver..."<<std::endl;
		int initial_state=t_states::state_menu;
		dfwimpl::state_driver sd(config, log_app, (*env), initial_state);

		//Setting the state according to the command line...
		if(carg.exists("-s") && carg.arg_follows("-s")) {
			sd.startup_set_state(std::atoi(carg.get_following("-s").c_str()));
		}

		lm::log(log_app).info()<<"init state driver..."<<std::endl;
		sd.init(kernel);
		sd.start(kernel);

		lm::log(log_app).info()<<"finish main proccess"<<std::endl;
	}
	catch(std::exception& e) {
		std::cout<<"Interrupting due to exception: "<<e.what()<<std::endl;
		lm::log(log_app).error()<<"an error happened "<<e.what()<<std::endl;
		lm::log(log_app).info()<<"stopping sdl2..."<<std::endl;
		ldt::sdl_shutdown();

		return 1;
	}

	lm::log(log_app).info()<<"stopping sdl2..."<<std::endl;
	ldt::sdl_shutdown();

	return 0;
}

std::unique_ptr<appenv::env> make_env(
	lm::logger& _logger
) {

	auto result=std::unique_ptr<appenv::env>(
		new app::env(_logger)
	);

	result->create_user_dir();
	result->copy_from_app_to_home("data/config/config.json", "config.json");

#ifdef AS_APPIMAGE
	result->appimagefy();
#endif

	return result;
}
