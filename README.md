#TODO:
	- Add a cartesian camera example, with polys.
	- Comment the cartesian vs screen system.

	- Center character animation.
	- Check camera spatiable objects.
	- Skip camera test (set in camera...)

# Dependencies

- A compiler supporting the C++17 standard
- libdansdl2 (https://github.com/themarlboroman/libdansdl2)
	- lm (https://github.com/themarlboroman/log)
	- tools (https://github.com/themarlboroman/tools)
		- rapidjson (https://github.com/tencent/rapidjson)
- dfw (https://github.com/themarlboroman/dfw)

On linux I just use gcc (currently gcc 9.3.0) and cmake to build / install all deps, then cmake to build this project.

# Building on windows.

In the past I have built this project and its dependencies with ming32 (or mingw64-64. As of today, I find it much easier to use msys2 and pretend I am doing things the linux way. See "building with msys2" for a complete guide.

If you are using mingw32, bear in mind that the zlib1.dll bundled with the mingw development packages at https://www.libsdl.org/projects/SDL_image/ is out of date with regards to libpng. Basically this means that you can build the project, but it will not run. Download a more up to date version, such as the one at https://sourceforge.net/projects/uqm-mods/ and you will be ready to go. If I remember correctly, this does not apply to mingw64-w64.

# Basic framework.

The idea here is to have a very basic application so things can run with little copy and paste. It will also include a few features I tend to forget about.

There are a few controllers here:

- a very simple console with four commands or so (first try to implement one)
	- Demonstrates a very simple controller with text input.
- a little game-like thing in axonometric 2d perspective (think snes rpg).
	- Demonstrates screen-coordinates camera, implementation of application classes, use of animations and frames, screen representations, input, SAT collision, signal broadcasting...
- a game thing companion that displays texts along the previous controller.
	- Demonstrates multi-controller drawing and signal broadcasting.
- another very simple racing game.
	- Demonstrates cartesian coordinates, use of polygons and collisions.
- a very simple frames-per-second test, not accesible unless tampering with the command line.
- a very simple step test, (not accesible unless command line is used), created to see how the timestep keeps up in different computers.

The rest of this text file includes:
	- a getting started guide.
	- notion on coordinate systems.
	- "howto" for many things.
	- files and directory structure.
	- interesting examples in classes.

## Getting started:

- Install dependencies and compile them (libdansdl2, dfw, tools, log).
- Alter makefile_linux/win to set the right paths.

Now for the ugly part.

- chmod +x scripts/setup_fresh.sh
- scripts/setup_fresh.sh
- code your own stuff
- make -f makefile_[linux-win] all

## On coordinate systems.

Copied and pasted from the "camera.h" header file of libdansdl2:

"A particularly interesting feature is the coordinate system: in "screen",
the focus box will be assumed to be a rectangle with its origin in the top-left
corner its width extending right and its height extending down (just as the screen
coordinate system used on the library). In "cartesian", the origin is the
bottom-left corner and the "height" parameter "ascends". The choice of coordinate
system makes the camera act as a mediator between screen and application space.
Whatever the system chosen, all representations MUST be in "screen" form.
The camera will not perform the transformations itself... Even if this
transformation could be implemented into the framework, I'd rather not do it
and keep it simple. All the client code needs to do is invert the y axis
when creating a representation for a logic object.".

Which translates to:

1 - Code all your logic assuming whatever coordinate system you want. The
	geometry classes in libdansdl2 assume a cartesian space, but you may be
	used to the trope of 0.0 being the top-left.
2 - When it comes to draw your representations, first set the camera to
	the space you are using in the state. If you assume 0.0 is the top of the
	screen and 0.0 is above 0.10, use "screen". Else use "cartesian".
3 - Create your representations in 'screen' space, always. This might be
	confusing if you are using the "cartesian" system: the trick is to invert
	than Y axis. Seriously, just go to "test_poly.cpp" and look at the
	draw_polygon method. It takes in a ldt::polygon_2d in cartesian space and
	turns it into a ldv::polygon_representation by copying all its points and
	inverting the y axis.

## A few definitions:

- Controller:
	TODOz
- State driver:
	TODO
- Signal broadcasting.

## Howto

Here are a few things that need to be done in a regular basis.

### Add new controllers:

You can do the automatic or manual methods...

The automatic method, under Linux:

- from the root of the project:

	- cd scripts
	- ./create_controller.sh controllername.

The manual method:

- Create your files in class/controllers. Use the _template.* files for your convenience.
- Add the controller to _makefile_app. state_driver should depend on it. This is a good moment to try and see if it compiles.
- Register the controller in state_driver.*. Perhaps in prepare_state() too.
- Add the state to states.h, so it can be used from other controllers to request state changes.
- Test the controller by changing the default state in state_driver's constructor.

It is a bit clunky, but does not take more than 5 minutes...

### Run code once, at application startup.

The one thing that best matches this is to use your main state_driver under the
class/dfwimpl directory. Modify the constructor and have it call your own hooks.

### Register and draw TTF fonts.

Use the tools::ttf_manager class. You can have a manager for each of your
instances or use the same for all of them (for example, making it property of
the state driver).

The ttf_manager can be constructed with no arguments. Once constructed, use

bool ttf_manager::insert(const std::string& _alias, int _size, const std::string& _path);

to insert fonts, which will return true on success (false if the font was already
registered!).

When it comes to draw the font you will need both a registered ttf and a
drawable object:

//Get the font.
const auto& font=ttf_manager.get("myfontalias", 14);
//Create the drawable... More parameters are available!!!!.
ldv::ttf_representation txt(font, ldv::rgba8(255,255,255,255), "My text");
txt.draw(screen);

Alternatively, you may want to consider the layout classes if you want to skip
creating instances all the time.

### Do signal broadcasting between controllers and with the state_driver

There are a few classes in the dfw header "signal_broadcasting.h" which allow
to setup a signal system. The concepts are:

- a broadcast_signal: the message to be sent. It is virtual since each message
	may have its own data.
- a receiver: the entity that receives a message. It may later choose to discard
	or interpret it. Since the receiver is designed to work by composition,
	you will need to design and instance it, probably using a lambda to
	make your controller's data accesible to it.
- sender: the entity that sends.
- a dispatcher: registers all receivers and senders alike. Through it, the
	sender may send a signal that will be dispatched to all receivers.

The easiest thing to do here is:

- Create you own signal header (usually in the controllers directory). In there:
	- Extend the receiver with your own (so, for example, injects automatically).
	- Setup your signals, extended from the original.
- When you need to setup a sender:
	- In your state driver, inject the dispatcher (get_signal_dispatcher())
	into your controller.
	- In your controller, create a sender object (dfw::signal_broadcaster)
	and use the dispatcher to construct it.
	- Send a message through the sender object.
- When you need to setup a receiver:
	- In your state driver, inject the dispatcher (get_signal_dispatcher())
	into your controller.
	- In your controller, use your own receiver object with the dispatcher.
	- In your controller, set up a lambda function into the receiver so it
	forwards to your own controller logic.
	- In your controller, setup your controller logic for receiving a
	signal (it will usually involve checking the message type (get_type())
	and later doing static casting.

There's an example of this between the test_2d and test_2d_text controllers.

The fun part is that mostly everything should be able to latch to this system.
For example, the state driver partakes in it so the screen size or audio
properties can be updated. On this particular example the lambda is spiffed up
so the kernel can be captured.

### Use a menu

Menus are time consuming if done by hand, but they are also an important part
of a relatively polished application.

The tools::options_menu class allows for creating dynamic menus of a single depth
on the fly. The particulars of the class are documented but the most important
thing to know is how to parse one from a dnot file. There's an example in the
menu controller that basically goes:

	//Create a translation map.
	std::map<std::string, int>	translation_map;
	//Parse the menu and fill the translation map.
	tools::mount_from_dnot(tools::dnot_parse_file("data/app_data/menus.dat")["main"], menu, &translation_map);
	//Create a translation vector.
	std::vector<tools::options_menu<std::string>::translation_struct > trad;
	//Use the translation map to fill the translation vector.
	for(const auto& p: translation_map) trad.push_back({p.first, menu_localization.get(p.second)});
	//Translate with the translation vector.
	menu.translate(trad);

That's enough to mount and translate a menu. Now, representations are a
completely different matter (all that tools::options_menu contains is the data).
The end user is responsible to convert the data to representations. However,
this project has a "menu_representation" class that is very useful to both
create the representations and add events (change selection, navigate...).
Examples are in the code of the controller. It is easier to copy and paste
than to try and explain. The two things to understand here:

	- in tools::options_menu terms, get_name is the name of a property and
	get_title is its value.
	- the "menu_representation" class is just a framework that must be filled
	with six functions (for example, with lambdas)

		 - register name/value representations void(const T&, std::vector<ldv::representation*>&)
			T is the key type, in case some keys need special treatment.
			The vector must be filled with as many representations
			as needed.
		 - draw name/value (const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)
			T is the key again, and size_t is the index we are currently
			drawing (two ways of referencing the same thing). The
			vector contains the representations created previously
			in the order of insertion (they need to be static_cast,
			of course). The bool indicates if this is the current
			selection.
		 - step name/value (const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)
			Similar to the previous one, to be called if the menu
			has "time" effects. Float is the delta value. This is
			only needed if we are going to use the "step" function
			of the menu.
	- representations are refreshed only when:
		 - it is forced through a menu_representation.refresh() call.
		 - whenever next, previous or browse are invoked.
		 - if the step functions are used.

There are alternatives, like the tools::grid_list and tools::vertical list
but these need to be manually rolled... In any case, complex menu systems
(like one with submenus, specific actions taking place upon selection of an
option and so on) are sinonyms with grunt work... The menu controller is a good
example of things that can be done.

### Redefine keys.

This is bound to exist on every game... The particulars are complex and the
classes involved many, so just check the "menu" controller. There's an entire
world of stuff there.

### Draw a controller different than the active one:

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
executed at all, so the view will be frozen.

An example can be seen in the test_2d_text controller which requests text_2d to
be drawn and then overlays new elements.

### Use the tools::view_composer to create static views.

Static views can be composed with dnot files to avoid the need of hardcoded
values and recompilation. These are most useful for menus, presentation screens,
fixed graphics... There is an example in the test_2d_text controller, using the
file in "data/app_data/layouts.dat". Another is in the menu controller, which
does things like use external representations and manipulate representations
in the layout from the code.

The documentation of the class is fairly complete in any case.

### Add new inputs:

- Add it to the enum in class/input.h.
- In case you need a keyboard input:
	- Locate the sdl key mapping in SDL_keycode.h (locate SDK_scancode.h first, usually in /usr/include/SDL2).
	- Or use the showkey -a command (second result).
	- Remember, this is a scancode, not the ASCII value!.
- Add this mapping to data/config/config.dnot in the "input" sequence.
	- The first parameter is "type" (0 is keyboard).
	- The second is device number and
	- The third is the code.

- Map it in class/dfwimpl/state_driver.cpp's prepare_input().
- Use it in your code by referencing as indicated in class/input.h:
	- if(input.is_input_[down|pressed|up](input_app::your_input_here)) {...}

### Change application states (controllers).

- From the controller code use set_state(state), as in set_state(state_main). Use a state defined in states.h.
- From the state_driver use states.set(v), where v is a state defined in states.h. "states" is a protected property of the state_driver_interface.

There are examples of both: you can see it done in main.cpp/state_driver.cpp and in the controllers.

### Use "loop" and application step.

This is the main application flow:

- Do common step (virtual function for your own state driver).
- Do common input (virtual function for your own state driver).
- Begin consuming loop time:
	- Loop the input (now there are events... this is actually sdl_input::loop).
	- Loop the state. (this is the main loop of your state).
	- Process message queue.
	- Evaluate possible state change. Break out of this loop if needed.
	-
- If change state
	- Confirm state change.
- If not change state
	- Do fps loop init.
	- Draw state.
	- Update screen.
	- Do fps loop end.

Thus:

- Controller loop happens X times, as much as needed to fill N seconds of logic before drawing.
	- This time is measured by a ldt::fps_counter, property of the kernel.
	- The value "delta_step" on the kernel represents the 0.01 seconds of logic.
	- Once N time units of logic are run, the screen is refreshed.
	- The loop repeats with N being the time the screen took to refresh or a maximum value set in state_driver::get_max_timestep().

### Use and inject resource managers.

In order to obtain any resource (music, sound, texture...) first these must be
loaded within the framework. The files in "/data/resources" contain all these
statically loaded resources. Of course, resources can be loaded dinamically too
but it just doesn't pay for small applications with small memory prints.

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

### Play audio

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

- Always remember:
	- If you set a callback, clear when you are done with the channel.
	- If you monitor a channel, unmonitor when you are done with the channel.
	- If you "stop" a channel, you callback executes.

- About unlinking channels:
	- Unlink does not affect the real channel.
	- Unlink signals the developer (that is, YOU) that the channel should not be used.
	- If in doubt, use the audio_channel_safe class. It will throw if you mess up.
	- With long lived and unmonitored channels, your unlinked channel may
	end up linked to a channel used by another entity. That's actually ok
	as the relationship is not exclusive. It may, however, do unexpected
	things like your sounds not playing because other entity is using the
	real channel.

Now, a few rules about sound channels for things you might like to do in your
classes:

- When I am constructed:
	- My channel is linked
		=> Do so at constructor.
	- My channel is unlinked
		=> Leave things as they are.

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
	- ... I don't have a callback and I want to do something.
		=> Bad choice. Why do you have a lda::audio_channel member? You can use fire-and-forget techniques.

### Implement text input.

Getting the text input to work properly seems tricky but it is actually easy. Two important things:

- It is wise not to use the dli::sdl_input (implemented by dfw::input) buffer, but to build your own so you can keep control. A simple std::string will do.
- Control characters (backspace, enter...) do not translate to input, and must be controlled individually as keydown presses.

The "console" controller includes a full working example, which goes like this:

- On awakening: start text input (input().start_text_input();) and clear previous contents (this last part just in case).
- On loop:
	- Check text events if(input().is_event_text()) and add input().get_text_input() to the buffer.
	- Check non text events that are text related (backspace, enter) as regular key down events. Act upon the buffer.
- On sleeping: stop the text input (input().stop_text_input();) so it does not affect other controllers.

### Implement localization.

A very basic example of localization is shown in the test_2d controller. The class localization extends tools::base_localization. The rules are simple:

- Languages are represented by integers.
- Strings are stored in files named #file#.#id_language#.dat. Their format is fixed and simple.

## Files and directory structure.

The most relevant files are.

- /_makefile_app: This is the real makefile for the application. Things that need tinkering are:
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

## Interesting examples in the code:

- The use of the animation sheet class is found in class/app/player.cpp
- The use of the sprite sheet class is found in class/app/tile_decoration.cpp

## Building with msys2

This example assumes a virtual machine running windows 10 (surprisingly, it can be installed for free).

- go to msys2.org, download and run the installer (mind the path where you install, read what the website says!). Be patient, it might take a while.
- run msys now (it says to in the installer, just do it)
- by now you should be running a console that is familiar to linux users. On this console, run "pacman -Syu" (no quotes, of course) to update the package database and base packages. Confirm what necessary (including closing the terminal).
- now, from the menu, run "Msys2 MSYS" to access that console again. run "pacman -Su" (again, no quotes) to update the thing. This time the console will not close.
- you will need to install some more stuff with pacman so run "pacman -S --needed" on the following packages (it might ask you some questions, I went with the defaults):
	- base-devel (build stuff)
	- mingw-w64-x86_64-toolchain (build stuff)
	- mingw-w64-x86_64-cmake (cmake, to build the projects, please do not use the cmake package!!!!)
	- mingw64/mingw-w64-x86_64-SDL2 (SDL2 dependencies)
	- mingw64/mingw-w64-x86_64-SDL2_image (SDL2 dependencies)
	- mingw64/mingw-w64-x86_64-SDL2_mixer (SDL2 dependencies)
	- mingw64/mingw-w64-x86_64-SDL2_ttf (SDL2 dependencies)
	- mingw-w64-x86_64-rapidjson (rapidjson, there's a package here, no need to build and install it... besides, it would need tweaking).
	- git (so we can pull the repositories)
	- msys2-runtime-devel (provides sys/time.h, for the tools repository)
	- mingw-w64-x86_64-mesa
	- mingw64/mingw-w64-x86_64-glew
	- mingw64/mingw-w64-x86_64-freeglut
	A command to install everything would be 
		
		pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL2_image mingw64/mingw-w64-x86_64-SDL2_mixer mingw64/mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-rapidjson git mingw-w64-x86_64-mesa msys2-runtime-devel mingw64/mingw-w64-x86_64-freeglut mingw64/mingw-w64-x86_64-glew

	This might take a long while.
- if need be, you can search for packages with pacman -Ss #search#
- ok, now, bear in mind that this terminal is running mounted on wherever you installed MSYS2 (this is important when you want to find whatever it is that you build!!!).
- in MSYS2, go to a suitable directory and download the dependencies (and this, if need be).
	- git clone https://github.com/themarlboroman/log
	- git clone https://github.com/themarlboroman/tools
	- git clone https://github.com/themarlboroman/libdansdl2
	- git clone https://github.com/themarlboroman/libdansdl2-tools
	- git clone https://github.com/themarlboroman/dfw
	- git clone https://github.com/themarlboroman/dfw-jumpstart
- Ok, that's for setting up repositories and dependencies. Close the terminal and from the menu, open "MSYS2 Ming64 64-bit" but run is as an administrator!!
- cd into the place where you downloaded your stuff
- Now, in the same order you got them:
	- cd into the directory (cd log, for example)
	- mkdir build
	- cd build
	- cmake .. -G "MSYS Makefiles"
	- make install (just make for dfw-jumpstart!!!)
- for dfw-jumpstart, you may need to edit the CmakeLists.txt file and uncomment the "include_directories" for the libs.

### troubleshooting

- make sure you set your windows git configuration right so the NL becomes CR NL. The localization files will suffer greatly and the thing won't boot if you don't!.
- do not try to build rapidjson downloading it from source. Save you the trouble, use the pacman package.
- it is very important that you install the cmake version listed. "tools" will not build if not.
- it is very important that you run cmake .. -G "MSYS Makefiles" so it uses the MSYS Makefiles generator.
- you MUST run the mingw64-bit as an administrator to install stuff.
