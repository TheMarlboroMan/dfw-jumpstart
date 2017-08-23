#TODO:
	- Add examples of sound.
		- With footsteps. Linked to frames.
	- Add examples of music.
	- Add example of menu.
	- Add example of localisation.
	- Add a cartesian camera example, with polys.
	- Comment the cartesian vs screen system.

	- Center character animation.
	- Check camera spatiable.
	- Skip camera test (set in camera...)
	- Add objects, to sort (bed, table, power pilon...)


#Basic framework.

The idea here is to have a very basic application so things can run with little copy and paste. It will also include a few features I tend to forget about.

There are a few controllers here: 

- a very simple console with four commands or so (first try to implement one) 
	- Demonstrates a very simple controller with text input.
- a little game-like thing in axonometric 2d perspective (think snes rpg). 
	- Demonstrates screen-coordinates camera, implementation of application classes, use of animations and frames, screen representations, input, SAT collision...
- a very simple frames-per-second test, not accesible.
	- 

##Getting started:

- Install dependencies and compile them (libdansdl2, dfw, herramientas_proyecto).
- Alter makefile_linux/win to set the right paths.
- Delete the example controllers and class files.
- Delete the example controllers and classes from _makefile_app (both in dependencies and instructions).
- Delete the example controllers from the state_driver files (includes and register_controllers).
- Modify what you need.
- Make with make -f makefile_[linux-win] all

##Howto

Here are a few things that need to be done in a regular basis.

###Add new controllers:

- Create your files in class/controllers. Use the _template.* files for your convenience.
- Add the controller to _makefile_app. state_driver should depend on it. This is a good moment to try and see if it compiles.
- Register the controller in state_driver.*. Perhaps in prepare_state() too.
- Add the state to states.h, so it can be used from other controllers to request state changes.
- Test the controller by changing the default state in state_driver's constructor.

It is a bit clunky, but does not take more than 5 minutes.

###Add new input:

- Add it to the enum in class/input.h.
- In case you need a keyboard input, locate the sdl key mapping in SDL_keycode.h (locate SDK_scancode.h first, usually in /usr/include/SDL2).
- Add this mapping to data/config/config.dnot in the "input" sequence. The first parameter is "type" (0 is keyboard), the second is device number and the third is the code.
- Map it in kernel_config.cpp's get_input_pairs().
- Use it in your code by referencing as indicated in class/input.h: 
	- if(input.is_input_[down|pressed|up](input_app::your_input_here)) {...}

###Change application states (controllers).

- From the controller code use set_state(state), as in set_state(state_main). Use a state defined in states.h. 

###Use "loop", "preloop", "postloop" and application step.

This is the main application flow:

- Do common step (virtual function for your own state driver).
- Do common input (virtual function for your own state driver).
- Preloop of your state (input has yet not looped, thus there are not events accessible).
- Begin consuming loop time:
	- Loop the input (now there are events... this is actually sdl_input::loop).
	- Loop the state. (this is the main loop of your state).
	- Audio queue.
	- Evaluate possible state change. Break out of this loop if needed.
- If change state
	- Confirm state change.
- If not change state
	- Postloop of your state. 
	- Draw state.

Thus:

- Preloop happens once before loop time is consumed.
- Postloop happens right before drawing the state.
- Controller loop happens X times, as much as needed to fill N seconds of logic.
	- This time is measured by a ldt::fps_counter, property of the kernel. 
	- The value "delta_step" on the kernel represents the 0.01 seconds of logic.
	- Once N seconds of logic are run, the screen is refreshed.
	- The loop repeats with N being the time the screen took to refresh or a maximum value set in state_driver::get_max_timestep().	

###Implement text input.

Getting the text input to work properly seems tricky but it is actually easy. Two important things:

- It is wise not to use the dli::sdl_input (implemented by dfw::input) buffer, but to build your own so you can keep control. A simple std::string will do.
- Control characters (backspace, enter...) do not translate to input, and must be controlled individually as keydown presses.

The "console" controller includes a full working example, which goes like this:

- On awakening: start text input (input().start_text_input();) and clear previous contents (this last part just in case).
- On loop:
	- Check text events if(input().is_event_text()) and add input().get_text_input() to the buffer.
	- Check non text events that are text related (backspace, enter) as regular key down events. Act upon the buffer.
- On sleeping: stop the text input (input().stop_text_input();) so it does not affect other controllers.

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

The organisation of the class and controller files is non prescriptive. As far as you are concerned you can have all your controllers, application classes and implementation of the framework on the root directory and everything will be allright.

##Interesting examples in the code:

- The use of the animation sheet class is found in class/app/player.cpp
- The use of the sprite sheet class is found in class/app/tile_decoration.cpp
