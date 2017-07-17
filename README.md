#Basic framework.

The idea here is to have a very basic application so things can run with little copy and paste.

There are two controllers here: a very simple console with four commands or so (first try to implement one) and a little game-like thing.

##Getting started:

- Install dependencies and compile them (libdansdl2, dfw, herramientas_proyecto).
- Alter makefile_linux/win to set the right paths.
- Delete the example controllers and class files.
- Delete the example controllers and classes from _makefile_app (both in dependencies and instructions).
- Delete the example controllers from the state_driver files (includes and register_controllers).
- Modify what you need.
- Make with make -f makefile_[linux-win] all

##Howto

###Add new controllers:

- Create your files in class/controllers. Use the _template.* files for your convenience.
- Add the controller to _makefile_app. state_driver should depend on it. This is a good moment to try and see if it compiles.
- Register the controller in state_driver.*. Perhaps in prepare_state() too.
- Add the state to states.h, so it can be used from other controllers to request state changes.
- Test the controller by changing the default state in state_driver's constructor.

###Add new input:

- Add it to the enum in class/input.h.
- Locate the sdl key mapping in SDL_keycode.h (locate SDK_scancode.h first, usually in /usr/include/SDL2).
- Add this mapping to data/config/config.dnot in the "input" sequence. The first parameter is "type" (0 is keyboard), the second is device number and the third is the code.
- Map it in kernel_config.cpp's get_input_pairs().
- Use it in your code by referencing as indicated in class/input.h

###Change application states (controllers).

- Use set_state(state), as in set_state(state_main). Use a state defined in states.h

##Files and directory structure.

The most relevant files are.

- /_makefile_app: This is the real makefile for the application. Things that need tingling are:
	- App dependencies: a list of all your controllers and classes, to make sure they are compiled before building the executable file.
	- External dependencies: Tools need to be added if they are to be used.
	- Implementation of DFW: state_driver will depend on all your controllers.
	- Controllers: You controllers go here.
	- Class: Your application classes go here.
- makefile_[common-linux-win]: These need no modification except for doing path configuration.
- /class/input.h: Definition of application inputs in a simple enum. Shorthand commodities. Modifiy each time a new input is added.
- /class/app: Your application classes go here.
- /class/controllers: All your controllers go here.
	- states.h : A simple enum with all states (controllers) for your application, shorthand commodities.
	- _template.*: Example files, for copy+paste convenience :).
- /class/dfwimpl: Implementation of dfw.
	- app_config.h: modify get_file_path() if you wish to change your config file location.
	- kernel_config.cpp: get_input_pairs() should be modified if you wish to add new inputs.
	- state_driver.h: all your controllers should be included here. If there are specific properties for the main driver they go here too.
	- state_driver.cpp: controllers must be registered in register_controllers().
- /data/config/config.dnot: main configuration file. New inputs must be registered here.
- /data/resources: all static resource files (music, sounds, textures...). # denotes a comment. Examples are included. Surfaces are not likely to be used, textures are the new thing instead.

#TODO:
	- Peek around all files. See if there's anything that can be streamlined.
		- I Particularly dislike how many things you need to change to add a new input (kernel config, config file, input file). Not neccesarily wrong per se but cannot be automated further.
	- Add examples of class logic.
	- Add examples of sound.
	- Add examples of music.
	- Add examples of rasterised resources.
