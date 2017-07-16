#Basic framework.

The idea here is to have a very basic application so things can run with little copy and paste.

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

###Add new input:


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
	- Put this in a repo when working.
	- Add examples of two controllers, so there can be changes.
	- Add examples of fonts.
	- Add examples of class logic.
	- Add examples of sound.
	- Add examples of music.
	- Add examples of input.
	- Add examples of rasterised resources.
	- Add examples of input.
