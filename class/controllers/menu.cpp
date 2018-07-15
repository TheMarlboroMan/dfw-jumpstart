#include "menu.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "signals.h"
#include "../input.h"
#include "../app/audio_defs.h"

using namespace app;

controller_menu::controller_menu(shared_resources& s, dfw::signal_dispatcher& sd, app_config& c)
	:s_resources(s), config(c), broadcaster(sd),
	main_menu_rep(main_menu),
	options_menu_rep(options_menu),
	controls_menu_rep(controls_menu),
	menu_localization(0, {"data/app_data/localization/texts"}),
	key_held_time{0.f},
	flicker{false, false, 1.f}
{


	create_functions();
	mount_menus();
	mount_layout();
	choose_current_menu(main_menu_rep);
}

void controller_menu::loop(dfw::input& input, const dfw::loop_iteration_data& lid)
{
	if(input().is_exit_signal())
	{
		set_leave(true);
		return;
	}

	if(input().is_event_input_with_pressed())
	{
		if(current_menu_ptr==&main_menu_rep) 		do_main_menu_input(input);
		else if(current_menu_ptr==&options_menu_rep) 	do_options_menu_input(input, lid.delta);
		else if(current_menu_ptr==&controls_menu_rep) 	do_controls_menu_input(input);
	}

	//There are a few steps...
	current_menu_ptr->step(lid.delta);
	for(auto& b : menu_decorations) b.step(lid.delta);
	pulse.step(lid.delta);
	flicker.step(lid.delta);
	if(flicker.changed)
	{
		tools::int_generator g(0, 300);
		auto &r=*(layout.get_by_id("code_text"));
		r.set_visible(flicker.visible);
		r.go_to(ldv::point(g(), g()));
		//TODO: It would be cool to play a sound just as it changes.
	}
}

void controller_menu::draw(ldv::screen& screen, int fps)
{
	layout.draw(screen);
	ldv::ttf_representation txtfps{s_resources.get_ttf_manager().get("consola-mono", 16), ldv::rgba8(255, 255, 255, 255), compat::to_string(fps), 1.};
	txtfps.draw(screen);
}

//Logic for the main menu.
//An interesting thing here is to duplicate the checks, because inputs can
//be redefined: we check for the app input but also for the fixed scancode.
void controller_menu::do_main_menu_input(dfw::input& input)
{
 	if(input.is_input_down(input_app::escape))						set_leave(true);
	else if(input.is_input_down(input_app::up) || input().is_key_down(SDL_SCANCODE_UP)) 	main_menu_rep.previous();
	else if(input.is_input_down(input_app::down) || input().is_key_down(SDL_SCANCODE_DOWN)) main_menu_rep.next();
	else if(input.is_input_down(input_app::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		switch(main_menu_rep.get_current_index()) //An alternative is to use get_current_key() and use std::strings.
		{
			case 0: //Restart. As a courtesy we force selection of "continue", in case we return.
				broadcaster.send_signal(signal_reset_state{});
				started=true;
				main_menu_rep.select_option("20_CONTINUE");
				set_state(state_test_2d);
			break;
			case 1: //Continue...
				if(started) set_state(continue_state);
			break;
			case 2: choose_current_menu(controls_menu_rep);	break;
			case 3: choose_current_menu(options_menu_rep);	break;
			case 4: set_leave(true); break;
		}
	}
}

//Logic for the controls menu...
void controller_menu::do_controls_menu_input(dfw::input& input)
{
 	if(input.is_input_down(input_app::escape))							choose_current_menu(main_menu_rep);
	else if(input.is_input_down(input_app::up) || input().is_key_down(SDL_SCANCODE_UP)) 		controls_menu_rep.previous();
	else if(input.is_input_down(input_app::down) || input().is_key_down(SDL_SCANCODE_DOWN)) 	controls_menu_rep.next();
	else if(input.is_input_down(input_app::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		switch(controls_menu_rep.get_current_index())
		{
			default: learn_control(input); break;
			case 5: restore_default_controls(input); break;
			case 6:
				broadcaster.send_signal(signal_save_controls{});
				choose_current_menu(main_menu_rep);
			break;
		}
	}
}

//Logic for the options menu...
void controller_menu::do_options_menu_input(dfw::input& input, float delta)
{
	auto f_dir=[delta, this](int dir) -> void
	{
		if(!key_held_time) s_resources.get_audio().play_sound(s_resources.get_audio_resource_manager().get_sound(sound_defs::menu_change));

		if(key_held_time > 0.4f || !key_held_time)
		{
			options_menu_rep.browse_current(dir);
			update_options_value(options_menu_rep.get_current_key());
		}
		key_held_time+=delta;
	};

	//f_dir is returned so key_held_time is not reset.
 	if(input.is_input_down(input_app::escape))							choose_current_menu(main_menu_rep);
	else if(input.is_input_down(input_app::up) || input().is_key_down(SDL_SCANCODE_UP)) 		options_menu_rep.previous();
	else if(input.is_input_down(input_app::down) || input().is_key_down(SDL_SCANCODE_DOWN)) 	options_menu_rep.next();
	else if(input.is_input_pressed(input_app::left) || input().is_key_pressed(SDL_SCANCODE_LEFT)) 	return f_dir(-1);
	else if(input.is_input_pressed(input_app::right) || input().is_key_down(SDL_SCANCODE_RIGHT)) 	return f_dir(1);
	else if(input.is_input_down(input_app::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		if(options_menu_rep.get_current_index()==4)
		{
			broadcaster.send_signal(signal_save_configuration{});
			choose_current_menu(main_menu_rep);
		}
	}

	key_held_time=0.f;
}

void controller_menu::learn_control(dfw::input& input)
{
	int it=0;
	switch(controls_menu_rep.get_current_index())
	{
		case 0: it=input_app::up; break;
		case 1: it=input_app::down; break;
		case 2: it=input_app::left; break;
		case 3: it=input_app::right; break;
		case 4: it=input_app::activate; break;
	}

	//This is the input learning function...
	auto f=[this, &input, it](SDL_Event& e, ldi::sdl_input::tf_default& df)
	{
		dfw::input_description::types t=dfw::input_description::types::none;
		int code=0, device=0;

		if(e.type==SDL_KEYDOWN)
		{
			t=dfw::input_description::types::keyboard;
			code=e.key.keysym.scancode;
		}
		else if(e.type==SDL_MOUSEBUTTONDOWN)
		{
			t=dfw::input_description::types::mouse;
			code=e.button.button;
		}
		else if(e.type==SDL_JOYBUTTONDOWN)
		{
			t=dfw::input_description::types::joystick;
			code=e.jbutton.button;
			device=input().get_joystick_index_from_id(e.jbutton.which);
		}
		else //Any other thing and we resort to the default treatment.
		{
			df(e);
			return true;
		}

		//Clear and configure...
		input.clear(it);
		input.configure(input.from_description({t, code, device}, it));

		//Set the new value, force refresh.
		controls_menu.set_string(controls_menu_rep.get_current_key(), translate_input(input.locate_description(it)));
		controls_menu_rep.refresh();

		//End the "learn" mode.
		input().reset_event_processing_function();
		return true;
	};

	//Begin the "learn" mode.
	input().set_event_processing_function(f);

	//Show a "learning" message...
	controls_menu.set_string(controls_menu_rep.get_current_key(), menu_localization.get(1066));
	controls_menu_rep.refresh();
}

void controller_menu::restore_default_controls(dfw::input& input)
{
	auto f=[&input, this](int it, int code, const std::string& key)
	{
		input.clear(it);
		input.configure(input.from_description({dfw::input_description::types::keyboard, code, 0}, it));
		controls_menu.set_string(key, translate_input(input.locate_description(it)));
	};

	f(input_app::up, SDL_SCANCODE_UP, "10_UP");
	f(input_app::down, SDL_SCANCODE_DOWN, "20_DOWN");
	f(input_app::left, SDL_SCANCODE_LEFT, "30_LEFT");
	f(input_app::right, SDL_SCANCODE_RIGHT, "40_RIGHT");
	f(input_app::activate, SDL_SCANCODE_SPACE ,"50_ACTIVATE");
	controls_menu_rep.refresh();
}

//Sends signal to update a particular options key.
void controller_menu::update_options_value(const std::string& key)
{
	if(key=="10_VIDEO_SIZE")
		broadcaster.send_signal(signal_video_size(options_menu.get_value_templated<std::string>(key)));
	else if(key=="25_VIDEO_VSYNC")
		broadcaster.send_signal(signal_video_vsync{options_menu.get_value_templated<bool>(key)});
	else if(key=="30_SOUND_VOLUME")
		broadcaster.send_signal(signal_audio_volume{options_menu.get_int(key)});
	else if(key=="40_MUSIC_VOLUME")
		broadcaster.send_signal(signal_music_volume{options_menu.get_int(key)});
}


//Creates all functions needed to draw the menus...
void controller_menu::create_functions()
{
	const auto& ttfm=s_resources.get_ttf_manager();
	const auto& rpul=pulse; //The alpha of the font pulsates...
	const auto& loc=menu_localization;
	using alh=ldv::representation_alignment::h;
	using alv=ldv::representation_alignment::v;

	//functions to register representations...

	register_funcs[rf_txt]=[ttfm](const std::string& /*k*/, std::vector<ldv::representation*>& v)
	{
		v.push_back(new ldv::ttf_representation{
			ttfm.get("consola-mono", 16),
			ldv::rgba8(255, 255, 255, 255), "", 1.});
	};

	register_funcs[rf_empty]=[](const std::string&, std::vector<ldv::representation*>&){};

	//functions to draw.

	auto draw_txt=[](ldv::ttf_representation& r, size_t index, const std::string& val, bool current, ldv::ttf_representation::text_align al)
	{
		r.lock_changes();
			r.set_color(current ? ldv::rgb8(255, 255, 255) : ldv::rgb8(192, 0, 0));
			r.set_text(val);
			r.set_text_align(al);
		r.unlock_changes();
		r.go_to({0, 20+(int)index*20});
	};

	//Draws a menu that is key only... Like the main one.
	draw_funcs[df_txt_right_single]=[draw_txt, &loc](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		draw_txt(r, index, val, current, ldv::ttf_representation::text_align::left);
		r.align({0,0,700,1}, {alh::inner_right, alv::none, 20,0});
	};

	//Draws key part of a menu with key and value.
	draw_funcs[df_txt_left_composite]=[draw_txt](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		draw_txt(r, index, val, current, ldv::ttf_representation::text_align::right);
		r.align({0,0,450,1}, {alh::inner_right, alv::none, 0, 0});
	};

	//Draws the value part of a menu with key and value.
	draw_funcs[df_txt_right_composite]=[draw_txt](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		if(!v.size()) return; //Some options may not have a value...
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		draw_txt(r, index, val, current, ldv::ttf_representation::text_align::left);
		r.align({470,0,700,1}, {alh::inner_left, alv::none, 0, 0});
	};

	draw_funcs[df_empty]=[](const std::string& , size_t, const std::vector<ldv::representation*>&, const std::string&, bool) {};

	//functions to step.

	step_funcs[sf_pulse]=[&rpul](const std::string& /*k*/, size_t /*index*/, float /*delta*/, const std::vector<ldv::representation*>& v, const std::string& /*val*/, bool current)
	{
		if(!v.size()) return; //Again, some options may not have a value.
		v[0]->set_alpha(current ? 255 : rpul.val);
	};

	step_funcs[sf_empty]=[](const std::string&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool) {};
}

//Mounts the menus, load values, use draw functions.
void controller_menu::mount_menus()
{
	const auto& loc=menu_localization;

	auto mount_menu=[&loc](tools::options_menu<std::string>& m, const std::string& file, const std::string& key)
	{
		std::map<std::string, int>	translation_map;
		tools::mount_from_dnot(tools::dnot_parse_file(file)[key], m, &translation_map);
		std::vector<tools::options_menu<std::string>::translation_struct > trad;
		for(const auto& p: translation_map) trad.push_back({p.first, loc.get(p.second)});
		m.translate(trad);
	};

	auto init_menu=[](menu_representation<std::string>& m,
		menu_representation<std::string>::tfunc_register f1, menu_representation<std::string>::tfunc_register f2,
		menu_representation<std::string>::tfunc_draw f3, menu_representation<std::string>::tfunc_draw f4,
		menu_representation<std::string>::tfunc_step f5, menu_representation<std::string>::tfunc_step f6)
	{
		m.set_register_name_function(f1); 	m.set_register_value_function(f2);
		m.set_draw_name_function(f3);		m.set_draw_value_function(f4);
		m.set_step_name_function(f5);		m.set_step_value_function(f6);
		m.init();
	};

	//Main menu...
	mount_menu(main_menu, "data/app_data/menus.dat", "main");
	init_menu(main_menu_rep, register_funcs[rf_txt], register_funcs[rf_empty],
			draw_funcs[df_txt_right_single], draw_funcs[df_empty],
			step_funcs[sf_pulse], step_funcs[sf_empty]);

	//Options menu..
	mount_menu(options_menu, "data/app_data/menus.dat", "options");


	std::string window_size=config.bool_from_path("config:video:fullscreen")
		? "fullscreen"
		: compat::to_string(config.int_from_path("config:video:window_w_px"))
				+"x"+compat::to_string(config.int_from_path("config:video:window_h_px"));

	options_menu.set_by_value_templated("10_VIDEO_SIZE", window_size);
	options_menu.set_by_value_templated("25_VIDEO_VSYNC", config.get_screen_vsync());
	options_menu.set_int("30_SOUND_VOLUME", config.get_audio_volume());
	options_menu.set_int("40_MUSIC_VOLUME", config.get_music_volume());

	init_menu(options_menu_rep, register_funcs[rf_txt], register_funcs[rf_txt],
			draw_funcs[df_txt_left_composite], draw_funcs[df_txt_right_composite],
			step_funcs[sf_pulse], step_funcs[sf_pulse]);

	//Control menus...
	mount_menu(controls_menu, "data/app_data/menus.dat", "controls");

	controls_menu.set_string("10_UP", translate_input(input_description_from_config_token(config.token_from_path("config:input:up"))));
	controls_menu.set_string("20_DOWN", translate_input(input_description_from_config_token(config.token_from_path("config:input:down"))));
	controls_menu.set_string("30_LEFT", translate_input(input_description_from_config_token(config.token_from_path("config:input:left"))));
	controls_menu.set_string("40_RIGHT", translate_input(input_description_from_config_token(config.token_from_path("config:input:right"))));
	controls_menu.set_string("50_ACTIVATE", translate_input(input_description_from_config_token(config.token_from_path("config:input:activate"))));

	init_menu(controls_menu_rep, register_funcs[rf_txt], register_funcs[rf_txt],
			draw_funcs[df_txt_left_composite], draw_funcs[df_txt_right_composite],
			step_funcs[sf_pulse], step_funcs[sf_pulse]);
}

//Translates a token from the config file (input part) to std::string, human readable.
std::string controller_menu::translate_input(const dfw::input_description& id)
{
	std::string res;

	switch(id.type)
	{
		case dfw::input_description::types::none:
			res=menu_localization.get(1104);
		break;
		case dfw::input_description::types::keyboard:
			res=menu_localization.get(1100)+" "+std::string(SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)id.code)));
		break;
		case dfw::input_description::types::joystick:
			res=menu_localization.get(1101)+" "+compat::to_string(id.device)+" "+menu_localization.get(1102)+" "+compat::to_string(id.code);
		break;
		case dfw::input_description::types::mouse:
			res=menu_localization.get(1103)+" "+menu_localization.get(1102)+" "+compat::to_string(id.code);
		break;
	}

	return res;
}

//Mounts the layout.
void controller_menu::mount_layout()
{
	//Register external representations and fonts...
	layout.register_as_external("menu_main", main_menu_rep.get_representation());
	layout.register_as_external("menu_options", options_menu_rep.get_representation());
	layout.register_as_external("menu_controls", controls_menu_rep.get_representation());

	//Finally, mount the layout...
	layout.map_font("code_font", s_resources.get_ttf_manager().get("consola-mono", 12));
	layout.parse("data/app_data/layouts.dat", "main_menu_layout");

	//...and get references to its data...
	menu_decorations.push_back({*(layout.get_by_id("boxr")), 0.f, 0.8f});
	menu_decorations.push_back({*(layout.get_by_id("boxg")), 0.3f, 0.4f});
	menu_decorations.push_back({*(layout.get_by_id("boxb")), 0.6f, 1.2f});
}

//Sets visibility of menus and updates the current_menu_ptr...
void controller_menu::choose_current_menu(menu_representation<std::string>& m)
{
	main_menu_rep.get_representation().set_visible(false);
	options_menu_rep.get_representation().set_visible(false);
	controls_menu_rep.get_representation().set_visible(false);

	//Manually set the first option of these...
	options_menu_rep.select_option(0);
	controls_menu_rep.select_option(0);

	current_menu_ptr=&m;
	current_menu_ptr->get_representation().set_visible(true);
}
