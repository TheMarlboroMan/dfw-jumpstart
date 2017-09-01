#TODO:
	- Add example of menu.
	- Add a cartesian camera example, with polys.
	- Comment the cartesian vs screen system.

	- Center character animation.
	- Check camera spatiable objects.
	- Skip camera test (set in camera...)


#Basic framework.

The idea here is to have a very basic application so things can run with little copy and paste. It will also include a few features I tend to forget about.

There are a few controllers here: 

- a very simple console with four commands or so (first try to implement one) 
	- Demonstrates a very simple controller with text input.
- a little game-like thing in axonometric 2d perspective (think snes rpg). 
	- Demonstrates screen-coordinates camera, implementation of application classes, use of animations and frames, screen representations, input, SAT collision, message broadcasting...
- a game thing companion that displays texts along the previous controller.
	- Demonstrates multi-controller drawing and message broadcasting.
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

##On coordinate systems.

//TODO.

##Howto

Here are a few things that need to be done in a regular basis.

###Add new controllers:

- Create your files in class/controllers. Use the _template.* files for your convenience.
- Add the controller to _makefile_app. state_driver should depend on it. This is a good moment to try and see if it compiles.
- Register the controller in state_driver.*. Perhaps in prepare_state() too.
- Add the state to states.h, so it can be used from other controllers to request state changes.
- Test the controller by changing the default state in state_driver's constructor.

It is a bit clunky, but does not take more than 5 minutes.

###Do message broadcasting between controllers:

Message broadcasting is a way to share scalar information between controllers,
like for example, strings, integers or data types that can be represented from
primitives. Controllers can be setup as broadcasters and receivers separatedly.

For each controller that can broadcast add this method (if you fail, an exception will be thrown when broadcasting).

	// message broadcasting

	public:

	virtual bool			is_broadcaster() const {return true;}

For each controller that can receive add these ones (if you fail to add a receive method an exception will be thrown when broadcasting).

	// message broadcasting

	public:

	virtual bool			is_receiver() const {return true;}
	virtual void			receive(const dfw::broadcast_message& msg)
	{
		//msg has two members: an int "type" and a dnot_token "tok"...
		//do what you need to do here.
	}

From a controller that can broadcast, do this:

	int type=1;
	tools::dnot_token tok=tools::dnot_parse_string{"data:{key: "value", thing: true}"};
	broadcast({type, tok});

An example can be seen in the test_2d controller which uses this information to broadcast text strings to test_2d_text.

If you need to share complex objects you can set up alternatives, like public
setters and getters in you controllers that can be accessed from your state_driver
"prepare_state" method.

###Draw a controller different than the active one:

There may be cases when you need to keep drawing a controller while executing one
(for example, a help screen about a certain controller). While this can be done 
setting up different states in controller and branching logic, input control
and drawing, there's an alternative. When you need it, add this function in 
the controller:

virtual void			request_draw(dfw::controller_view_manager& cvm) 
{
	//Do this to add another controller by index.
	cvm.add(1); 

	//Do this to add your controller.
	cvm.add_ptr(this);
}

And that's all. Controllers will be drawn in the declared order (in the example,
first the one with the index 1 and then "this"). No logic of the controllers will be
executed at all.

An example can be seen in the test_2d_text controller which requests text_2d to 
be drawn and then overlays new elements.

###Use the tools::view_composer to create static views.

Static views can be composed with dnot files to avoid the need of hardcoded 
values and recompilation. These are most useful for menus, presentation screens,
fixed graphics... There is an example in the test_2d_text controller, using the 
file in "data/app_data/layouts.dat". 

The documentation of the class if fairly complete in any case.

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
	- Process message queue.
	- Evaluate possible state change. Break out of this loop if needed.
	- 
- Postloop of your state.
- If change state
	- Confirm state change.
- If not change state
	- Do fps loop init.
	- Draw state.
	- Update screen.
	- Do fps loop end.

Thus:

- Preloop happens once before loop time is consumed.
- Postloop happens right before drawing the state.
- Controller loop happens X times, as much as needed to fill N seconds of logic.
	- This time is measured by a ldt::fps_counter, property of the kernel. 
	- The value "delta_step" on the kernel represents the 0.01 seconds of logic.
	- Once N seconds of logic are run, the screen is refreshed.
	- The loop repeats with N being the time the screen took to refresh or a maximum value set in state_driver::get_max_timestep().	

###Use and inject resource managers.

In order to obtain any resource (music, sound, texture...) first these must be
loaded within the framework. The files in "/data/resources" contain all these
statically loaded resources. Of course, resources can be loaded dinamically too but it just doesn't pay for
small applications with small memory prints. 

Each resource goes into a different resource manager, property of the kernel, 
thus accesible from the state_driver:

ldv::resource_manager&	kernel.get_video_resource_manager() provides access to
textures kernel.get_video_resource_manager().get_texture(index) and 
surfaces kernel.get_video_resource_manager().get_surface(index).

lda::resource_manager&	kernel.get_audio_resource_manager() does the same for
aural resources with "get_sound" and "get_music". 

These managers are emptied once the application stops. In order to access any
texture from any class or controller (which happens all the time) either the 
kernel must be accessible to these (bad idea) or the managers must be injected
into them (better idea). The de facto way of doing this is to set a reference
to them into your controller and pass them when the controllers are being
constructed in the state_driver. Controllers, in turn, can pass these references
to classes or methods.

Another option is the one used in this code: a single "shared_resources" object
that has references to all managers and shared stuff, injected into the 
controllers later. Shoddy, but quick.

###Play audio

The simplest way to play sound and music is to obtain the dfw::audio object 
from the kernel (kernel.get_audio()) and use its play_music and play_sound
methods, that map directly to the libdansdl2 audio_controller (check the
documentation). The play_sound method should be enclosed in a try-catch
block, as it will throw when no channels are available (for example, if you
play many sounds simultaneously... you can always request more channels anyway).
Since the kernel is not accessible from the controllers you will want to inject
this dfw::audio object into them... This would be the "fire and forget" method,
as no control over the played sounds is given.

To achieve control over the sounds (for example, to change their volume, panning
or stop them on the fly), a channel must be acquired from the 
lda::audio_controller class (accesible through the () method of the dfw::audio,
is in kernel.get_audio()()).

To acquire a channel use lda::audio_controller::get_channel(size_t) or 
lda::audio_controller::get_free_channel() (which may throw). This will
return an lda::audio_channel object that must instantly be set to play something 
or monitored (lest the same channel is returned again with the next call). 
This object is "linked" to a real_audio_channel so every action upon it will
reflect on the real channel. When the lda::audio_channel objects have long
lifetimes this can be problematic, as different objects may point to the same
real channel. 

A call to "unlink" will unlink the lda::audio_channel from
its real_audio_channel (any operation upon the channel will crash, as it is
equivalent to deferencing an invalid pointer). After "unlink" is called, the
channel should not be used except for reassigning it through

my_channel=lda::audio_controller::get_free_channel().

Channels will be reported as free if they are not playing. Any monitored channel
will not be reported as free. Once a channel is monitored, it won't be returned 
to the channel pool until it is unmonitored. Monitored, thus, basically means 
"Please, do not return this channel to the pool when it is done playing, I want 
to keep using it".

To control the lifetime of channels, it is possible to attach a callback
(derived from lda::audio_callback_interface) to the channel, whose method
operator() will be executed when the sound stops playing. This makes sense 
since you can:

- Acquire a channel.
- Monitor it (now the channel is yours).
- Set the callback.
- Play sounds.
- Once the sound is done, execute the callback.
- In the callback, clear the channel callback, unmonitor it and free it so it
can be acquired again.


Notice that the callback function will be called only if a lda::audio_callback_interface
is attached. It will be executed either when the sound ends "naturally" or 
when "stop" is called on the channel.

Also, notice these methods on the channel:

- free (private function).
	unmonitors the channel, removes panning effects, removes callback_listener...
- set_monitoring(false) 
	only unmonitors the channel. The rest is the same.
- stop:
	halts the sound and frees the channel (if unmonitored).
- do_callback (the callback execution, happens when stop is called or the sound ends)
	executes the callback listener. After that, if the channel is unmonitored,
	frees it.

Understanding these dynamics is key to properly manage sound channels. The
class "object_audio_player" implements these audio ideas, including callbacks
and monitoring. If improperly done, crashes are sure to happen.

When lda::audio_channel instances exist, this guide can help set them up. It is
done from the point of view of an application object that owns a
lda::audio_channel

- When I am constructed:
	- My channel is linked
		=> Do so at constructor.
	- My channel is unlinked
		=> Leave things as they are.

- My channel is ...

- When I am destroyed
	- My channel is playing and...
		- ... I want it to keep playing.
			=> If there's any call to "channel.unlink" in your code, 
			check if the channel is linked first!
			=> Unmonitor if monitored: on the contrary it will be 
			lost forever (not really, but well).
			=> Remove the callback if any: specially if it lies in 
			the scope of your class.
			=> You don't need to unlink it: it will do so upon destruction.
		- ... I want it to stop:
			=> Unmonitor if monitored. This never has side effects.
			=> If you don't want your callback to execute, remove it. 
			If your callback unlinks the channel you certainly want to do this, 
			as it will do so and then crash when the destructor continues.
			=> Call "stop"... Stop will call free. 

- When my audio channel ends playing ...
	- ... I have a callback ...
		- ... and I want to reuse the very same channel.
			=> Make sure you are monitoring the channel before playing it.
			=> Do not unlink your channel in your callback. Do so in your destructor.
			=> Do not unmonitor it.
		- ... I want to free the channel.
			=> If monitored, unmonitor it. "Free" will be called next.
			=> Remove the callback listener: else it may execute out of your scope or worse, crash.
			=> Unlink the channel.
	- ... I don't have a callback
		=> Bad choice. Why do you have a lda::audio_channel member?

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

###Implement localization.

A very basic example of localization is shown in the test_2d controller. The class localization extends tools::base_localization. The rules are simple:

- Languages are represented by integers.
- Strings are stored in files named #file#.#id_language#.dat.

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
