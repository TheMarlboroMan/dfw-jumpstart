#include "../../include/controller/menu.h"

//local
#include "../../include/input/input.h"

#include "../../include/controller/signals.h"

#include "../../include/app/audio_defs.h"

#include <ldv/ttf_representation.h>

#include <tools/json.h>
#include <tools/file_utils.h>

//std
#include <cassert>

using namespace controller;

menu::menu(app::shared_resources& s, dfw::signal_dispatcher& sd, dfwimpl::config& c)
	:s_resources(s), 
	config(c), 
	broadcaster(sd),
	main_menu_rep(nullptr),
	options_menu_rep(nullptr),
	controls_menu_rep(nullptr),
	menu_localization("data/app_data/localization", "en", {"texts.dat"}),
	key_held_time{0.f},
	flicker{false, false, 1.f} {

	create_functions();
	mount_menus();
	mount_layout();
	choose_current_menu(main_menu_rep.get());
}

void menu::loop(dfw::input& input, const dfw::loop_iteration_data& lid) {

	if(input().is_exit_signal()) {
		set_leave(true);
		return;
	}

	if(input().is_event_input_with_pressed()) {
		if(current_menu_ptr==main_menu_rep.get())	do_main_menu_input(input);
		else if(current_menu_ptr==options_menu_rep.get()) do_options_menu_input(input, lid.delta);
		else if(current_menu_ptr==controls_menu_rep.get()) do_controls_menu_input(input);
	}

	//There are a few steps...
	current_menu_ptr->step(lid.delta);
	for(auto& b : menu_decorations) {
		b.step(lid.delta);
	}
	pulse.step(lid.delta);
	flicker.step(lid.delta);
	if(flicker.changed) {

		tools::int_generator g(0, 300);
		auto &r=*(layout.get_by_id("code_text"));
		r.set_visible(flicker.visible);
		r.go_to(ldv::point(g(), g()));
		//TODO: It would be cool to play a sound just as it changes.
	}
}

void menu::draw(ldv::screen& screen, int fps) {
	layout.draw(screen);
	ldv::ttf_representation txtfps{s_resources.get_ttf_manager().get("consola-mono", 16), ldv::rgba8(255, 255, 255, 255), std::to_string(fps), 1.};
	txtfps.draw(screen);
}

//Logic for the main menu.
//An interesting thing here is to duplicate the checks, because inputs can
//be redefined: we check for the app input but also for the fixed scancode.
void menu::do_main_menu_input(dfw::input& input) {

 	if(input.is_input_down(input::escape))						set_leave(true);
	else if(input.is_input_down(input::up) || input().is_key_down(SDL_SCANCODE_UP)) 	main_menu_rep->previous();
	else if(input.is_input_down(input::down) || input().is_key_down(SDL_SCANCODE_DOWN)) main_menu_rep->next();
	else if(input.is_input_down(input::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		const auto current_key=main_menu_rep->get_current_key();

		//Restart. As a courtesy we force selection of "continue", in case we return.
		if(current_key=="10_START") {
				broadcaster.send_signal(signal_reset_state{});
				started=true;
				main_menu_rep->select_option("20_CONTINUE");
				set_state(state_test_2d);
		}
		else if(current_key=="20_CONTINUE") {
			if(started) set_state(continue_state);
		}
		else if(current_key=="30_CONTROLS") {
			choose_current_menu(controls_menu_rep.get());
		}
		else if(current_key=="40_OPTIONS") {
			choose_current_menu(options_menu_rep.get());
		}
		else if(current_key=="50_EXIT") {
			set_leave(true);
		}
	}
}

//Logic for the controls menu...
void menu::do_controls_menu_input(dfw::input& input)
{
 	if(input.is_input_down(input::escape))							choose_current_menu(main_menu_rep.get());
	else if(input.is_input_down(input::up) || input().is_key_down(SDL_SCANCODE_UP)) 		controls_menu_rep->previous();
	else if(input.is_input_down(input::down) || input().is_key_down(SDL_SCANCODE_DOWN)) 	controls_menu_rep->next();
	else if(input.is_input_down(input::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		const auto& key=controls_menu_rep->get_current_key();

		if(key=="55_RESTORE") {
			restore_default_controls(input); 
		}
		else if(key=="60_BACK") {
			broadcaster.send_signal(signal_save_controls{});
			choose_current_menu(main_menu_rep.get());
		}
		else {
			//TODO: Are you even doing this?????????
			learn_control(input);
		}
	}
}

//Logic for the options menu...
void menu::do_options_menu_input(dfw::input& input, float delta) {

	auto f_dir=[delta, this](int dir) -> void
	{
		if(!key_held_time) s_resources.get_audio().play_sound(s_resources.get_audio_resource_manager().get_sound(app::sound_defs::menu_change));

		if(key_held_time > 0.4f || !key_held_time) {

			options_menu_rep->browse_current(dir);
			update_options_value(options_menu_rep->get_current_key());
		}
		key_held_time+=delta;
	};

	if(input.is_input_down(input::escape))							choose_current_menu(main_menu_rep.get());
	else if(input.is_input_down(input::up) || input().is_key_down(SDL_SCANCODE_UP)) 		options_menu_rep->previous();
	else if(input.is_input_down(input::down) || input().is_key_down(SDL_SCANCODE_DOWN)) 	options_menu_rep->next();
	//f_dir is returned so key_held_time is not reset.
	else if(input.is_input_pressed(input::left) || input().is_key_pressed(SDL_SCANCODE_LEFT)) 	return f_dir(-1);
	else if(input.is_input_pressed(input::right) || input().is_key_down(SDL_SCANCODE_RIGHT)) 	return f_dir(1);
	else if(input.is_input_down(input::activate) || input().is_key_down(SDL_SCANCODE_RETURN))
	{
		if(options_menu_rep->get_current_key()=="50_BACK") {
			broadcaster.send_signal(signal_save_configuration{});
			choose_current_menu(main_menu_rep.get());
		}
	}

	key_held_time=0.f;
}

void menu::learn_control(dfw::input& input)
{
	int it=0;
	const auto key=controls_menu_rep->get_current_key();

	if(key=="10_UP") it=input::up;
	else if(key=="20_DOWN") it=input::down;
	else if(key=="30_LEFT") it=input::left;
	else if(key=="40_RIGHT") it=input::right;
	else if(key=="50_ACTIVATE") it=input::activate;

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
		controls_menu.set(controls_menu_rep->get_current_key(), translate_input(input.locate_description(it)));
		controls_menu_rep->refresh();

		//End the "learn" mode.
		input().reset_event_processing_function();
		return true;
	};

	//Begin the "learn" mode.
	input().set_event_processing_function(f);

	//Show a "learning" message...
	controls_menu.set(controls_menu_rep->get_current_key(), menu_localization.get("menu-1066"));
	controls_menu_rep->refresh();
}

void menu::restore_default_controls(dfw::input& input)
{
	auto f=[&input, this](int it, int code, const std::string& key)
	{
		input.clear(it);
		input.configure(input.from_description({dfw::input_description::types::keyboard, code, 0}, it));
		controls_menu.set(key, translate_input(input.locate_description(it)));
	};

	f(input::up, SDL_SCANCODE_UP, "10_UP");
	f(input::down, SDL_SCANCODE_DOWN, "20_DOWN");
	f(input::left, SDL_SCANCODE_LEFT, "30_LEFT");
	f(input::right, SDL_SCANCODE_RIGHT, "40_RIGHT");
	f(input::activate, SDL_SCANCODE_SPACE ,"50_ACTIVATE");
	controls_menu_rep->refresh();
}

//Sends signal to update a particular options key.
void menu::update_options_value(const std::string& _key) {

	if(_key=="10_VIDEO_SIZE") {
		broadcaster.send_signal(signal_video_size(options_menu.get_string(_key)));
		return;
	}

	if(_key=="25_VIDEO_VSYNC") {
		broadcaster.send_signal(signal_video_vsync{options_menu.get_bool(_key)});
		return;
	}


	if(_key=="30_SOUND_VOLUME") {
		broadcaster.send_signal(signal_audio_volume{options_menu.get_int(_key)});
		return;
	}

	if(_key=="40_MUSIC_VOLUME") {
		broadcaster.send_signal(signal_music_volume{options_menu.get_int(_key)});
		return;
	}
}

//Creates all functions needed to draw the menus...
void menu::create_functions()
{
	const auto& ttfm=s_resources.get_ttf_manager();

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

	using alh=ldv::representation_alignment::h;
	using alv=ldv::representation_alignment::v;

	//Draws a menu that is key only... Like the main one.
	draw_funcs[df_txt_right_single]=[draw_txt](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		draw_txt(r, index, val, current, ldv::ttf_representation::text_align::right);
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
	const auto& rpul=pulse; //The alpha of the font pulsates...
	step_funcs[sf_pulse]=[&rpul](const std::string& /*k*/, size_t /*index*/, float /*delta*/, const std::vector<ldv::representation*>& v, const std::string& /*val*/, bool current)
	{
		if(!v.size()) return; //Again, some options may not have a value.
		v[0]->set_alpha(current ? 255 : rpul.val);
	};

	step_funcs[sf_empty]=[](const std::string&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool) {};
}

//Mounts the menus, load values, use draw functions.
void menu::mount_menus()
{
	const auto& loc=menu_localization;

	auto mount_menu=[&loc](tools::options_menu<std::string>& m, const std::string& _file, const std::string& _key) {

		auto root=tools::parse_json_string(
			tools::dump_file(
				_file
			)
		);

		tools::options_menu_from_json(
			root[_key.c_str()].GetObject(),
			m
		);
	};

	//Main menu...
	mount_menu(main_menu, "data/app_data/menus.json", "main");
	main_menu_rep.reset(
		new app::menu_representation<std::string>{
			main_menu,
			register_funcs[rf_txt], register_funcs[rf_empty],
			draw_funcs[df_txt_right_single], draw_funcs[df_empty],
			step_funcs[sf_pulse], step_funcs[sf_empty],
			[this](const std::string& _k) -> std::string {

				if(_k=="10_START") return menu_localization.get("menu-1000");
				if(_k=="20_CONTINUE") return menu_localization.get("menu-1001");
				if(_k=="30_CONTROLS") return menu_localization.get("menu-1002");
				if(_k=="40_OPTIONS") return menu_localization.get("menu-1003");
				else return menu_localization.get("menu-1004");
			},
			[](const std::string&) -> std::string {
				return "";
			},
		}
	);

	//Options menu..
	mount_menu(options_menu, "data/app_data/menus.json", "options");

	std::string window_size=config.bool_from_path("video:fullscreen")
		? "fullscreen"
		: std::to_string(config.int_from_path("video:window_w_px"))
				+"x"+std::to_string(config.int_from_path("video:window_h_px"));

	options_menu.set("10_VIDEO_SIZE", window_size);
	options_menu.set("25_VIDEO_VSYNC", config.get_screen_vsync());
	options_menu.set("30_SOUND_VOLUME", config.get_audio_volume());
	options_menu.set("40_MUSIC_VOLUME", config.get_music_volume());

	options_menu_rep.reset(
		new app::menu_representation<std::string>{
			options_menu,
			register_funcs[rf_txt], register_funcs[rf_txt],
			draw_funcs[df_txt_left_composite], draw_funcs[df_txt_right_composite],
			step_funcs[sf_pulse], step_funcs[sf_pulse],
			[this](const std::string& _k) -> std::string {

				if(_k=="10_VIDEO_SIZE") return menu_localization.get("menu-1010");
				if(_k=="25_VIDEO_VSYNC") return menu_localization.get("menu-1025");
				if(_k=="30_SOUND_VOLUME") return menu_localization.get("menu-1030");
				if(_k=="40_MUSIC_VOLUME") return menu_localization.get("menu-1040");
				return menu_localization.get("menu-1050");
			},
			[this](const std::string& _k) -> std::string {

				if(_k=="10_VIDEO_SIZE") {
					const auto& v=options_menu.get_string(_k);
					if(v=="700x500") return menu_localization.get("menu-1011");
					if(v=="1100x750") return menu_localization.get("menu-1012");
					if(v=="1400x1000") return menu_localization.get("menu-1013");
					return menu_localization.get("menu-1014");
				}

				if(_k=="25_VIDEO_VSYNC") {
					return options_menu.get_bool(_k)
						? menu_localization.get("menu-1026")
						: menu_localization.get("menu-1027");
				}

				if(_k=="30_SOUND_VOLUME" || _k=="40_MUSIC_VOLUME") {
					return std::to_string(options_menu.get_int(_k));
				}

				return "";
			},
		}
	);

	//Control menus...
	mount_menu(controls_menu, "data/app_data/menus.json", "controls");

	controls_menu.set("10_UP", translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:up"))));
	controls_menu.set("20_DOWN", translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:down"))));
	controls_menu.set("30_LEFT", translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:left"))));
	controls_menu.set("40_RIGHT", translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:right"))));
	controls_menu.set("50_ACTIVATE", translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:activate"))));

	controls_menu_rep.reset(
		new app::menu_representation<std::string>{
			controls_menu,
			register_funcs[rf_txt], register_funcs[rf_txt],
			draw_funcs[df_txt_left_composite], draw_funcs[df_txt_right_composite],
			step_funcs[sf_pulse], step_funcs[sf_pulse],
			[this](const std::string& _key) -> std::string {

				if(_key=="10_UP") return menu_localization.get("menu-1060");
				if(_key=="20_DOWN") return menu_localization.get("menu-1061");
				if(_key=="30_LEFT") return menu_localization.get("menu-1062");
				if(_key=="40_RIGHT") return menu_localization.get("menu-1063");
				if(_key=="50_ACTIVATE") return menu_localization.get("menu-1064");
				if(_key=="55_RESTORE") return menu_localization.get("menu-1067");
				else return menu_localization.get("menu-1065");
			},
			[this](const std::string& _key) -> std::string {

				if(_key=="10_UP") return translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:up")));
				if(_key=="20_DOWN") return translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:down")));
				if(_key=="30_LEFT") return translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:left")));
				if(_key=="40_RIGHT") return translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:right")));
				if(_key=="50_ACTIVATE") return translate_input(dfwimpl::input_description_from_config_token(config.token_from_path("input:activate")));

				return "";
			}
		}
	);
}

//Translates a token from the config file (input part) to std::string, human readable.
std::string menu::translate_input(const dfw::input_description& id)
{
	std::string res;

	switch(id.type) {
		case dfw::input_description::types::none:
			res=menu_localization.get("menu-1104");
		break;
		case dfw::input_description::types::keyboard:
			res=menu_localization.get("menu-1100")+" "+std::string(SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)id.code)));
		break;
		case dfw::input_description::types::joystick:
			res=menu_localization.get("menu-1101")+" "+std::to_string(id.device)+" "+menu_localization.get("menu-1102")+" "+std::to_string(id.code);
		break;
		case dfw::input_description::types::mouse:
			res=menu_localization.get("menu-1103")+" "+menu_localization.get("menu-1102")+" "+std::to_string(id.code);
		break;
	}

	return res;
}

//Mounts the layout.
void menu::mount_layout()
{
	//Register external representations and fonts...
	layout.register_as_external("menu_main", main_menu_rep->get_representation());
	layout.register_as_external("menu_options", options_menu_rep->get_representation());
	layout.register_as_external("menu_controls", controls_menu_rep->get_representation());

	//Finally, mount the layout...
	layout.map_font("code_font", s_resources.get_ttf_manager().get("consola-mono", 12));

	auto root=tools::parse_json_string(
		tools::dump_file(
			"data/app_data/layouts.json"
		)
	);

	layout.parse(root["main_menu_layout"]);

	//...and get references to its data...
	menu_decorations.push_back({*(layout.get_by_id("boxr")), 0.f, 0.8f});
	menu_decorations.push_back({*(layout.get_by_id("boxg")), 0.3f, 0.4f});
	menu_decorations.push_back({*(layout.get_by_id("boxb")), 0.6f, 1.2f});
}

//Sets visibility of menus and updates the current_menu_ptr...
void menu::choose_current_menu(app::menu_representation<std::string>* _m) {

	main_menu_rep->get_representation().set_visible(false);
	options_menu_rep->get_representation().set_visible(false);
	controls_menu_rep->get_representation().set_visible(false);

	//Manually set the first option of these...
	options_menu_rep->select_option("10_VIDEO_SIZE");
	controls_menu_rep->select_option("10_UP");

	current_menu_ptr=_m;
	current_menu_ptr->get_representation().set_visible(true);
}
