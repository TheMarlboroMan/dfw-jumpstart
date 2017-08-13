#include "kernel_config.h"

//std
#include <iostream>

//sdl
#include <SDL2/SDL.h>

//libdansdl2
#include <input/sdl_input/sdl_input.h>

//tools
#include <class/text_reader.h>

//local
#include "../input.h"

using namespace app;

kernel_config::kernel_config(const app_config& c)
	:config(c)
{

}

std::vector<std::string> kernel_config::get_sound_entries() const
{
	return tools::explode_lines_from_file(std::string("data/resources/audio.txt"));
}

std::vector<std::string> kernel_config::get_music_entries() const
{
	return tools::explode_lines_from_file(std::string("data/resources/music.txt"));
}

std::vector<std::string> kernel_config::get_texture_entries() const
{
	return tools::explode_lines_from_file(std::string("data/resources/textures.txt"));
}

std::vector<std::string> kernel_config::get_surface_entries() const
{
	return tools::explode_lines_from_file(std::string("data/resources/surfaces.txt"));
}

dfw::window_info kernel_config::get_window_info() const
{
	return dfw::window_info{
		config.int_from_path("config:video:window_w_px"),
		config.int_from_path("config:video:window_h_px"),  
		config.int_from_path("config:video:window_w_logical"), 
		config.int_from_path("config:video:window_h_px"), 
		config.string_from_path("config:video:window_title"),
		config.bool_from_path("config:video:window_show_cursor"),
		config.bool_from_path("config:video:vsync")};
}

std::vector<dfw::input_pair> kernel_config::get_input_pairs() const
{
	auto type_from_config=[](int t)
	{
		using namespace dfw;

		switch(t)
		{
			case 0: return input_pair::types::keyboard; break;
			case 1: return input_pair::types::joystick; break;
			case 2: return input_pair::types::mouse; break;
		}

		return input_pair::types::keyboard;
	};

	using namespace dfw;

	std::map<int, app_config::user_input> usr_map {
		{input_app::left, 	config.token_from_path_input("left")},
		{input_app::right, 	config.token_from_path_input("right")},
		{input_app::up, 	config.token_from_path_input("up")},
		{input_app::down, 	config.token_from_path_input("down")},
		{input_app::console_newline, 	config.token_from_path_input("console_newline")},
		{input_app::console_backspace, 	config.token_from_path_input("console_backspace")}
	};

	std::vector<input_pair> res{input_pair{input_pair::types::keyboard, input_app::escape, SDL_SCANCODE_ESCAPE, 0}};
	for(const auto& p : usr_map) res.push_back({type_from_config(p.second.type), p.first, p.second.code, p.second.device});

	return res;
}
