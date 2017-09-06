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
	using namespace dfw;

	return std::vector<input_pair> {
		{{input_description::types::keyboard, SDL_SCANCODE_ESCAPE, 0}, input_app::escape},
		{input_description_from_config_token(config.token_from_path("config:input:left")), input_app::left},
		{input_description_from_config_token(config.token_from_path("config:input:right")), input_app::right},
		{input_description_from_config_token(config.token_from_path("config:input:up")), input_app::up},
		{input_description_from_config_token(config.token_from_path("config:input:down")), input_app::down},
		{input_description_from_config_token(config.token_from_path("config:input:activate")), input_app::activate},
		{input_description_from_config_token(config.token_from_path("config:input:console_newline")), input_app::console_newline},
		{input_description_from_config_token(config.token_from_path("config:input:console_backspace")), input_app::console_backspace}
#ifdef WDEBUG_CODE
		,{input_description_from_config_token(config.token_from_path("config:input:reload_debug_config")), input_app::reload_debug_config}
#endif
	};
}
