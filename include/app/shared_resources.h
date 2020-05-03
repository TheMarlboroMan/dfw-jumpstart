#pragma once

//The place where all resources live: animation tables, sprites, fonts, log...
//Many of these things are property of the kernel, but I don't fancy just 
//passing the kernel along, neither updating the framework to create a module
//specially tailored to this thing.

//Considering that this is a mismash of unrelated functionality, this is the
//culminating point of lazyness... But it is also easy and clear, mind you.

//Log

//Tools
#include <tools/chrono.h>
#include <tools/arg_manager.h>

//dfw
#include <dfw/audio.h>

//Libdan
#include <ldv/resource_manager.h>
#include <lda/resource_manager.h>

//ldtools
#include <ldtools/ttf_manager.h>
#include <ldtools/sprite_table.h>
#include <ldtools/animation_table.h>

#ifdef WDEBUG_CODE
#include <tools/json_config_file.h>
#include <rapidjson/document.h>
#endif

namespace app
{

class shared_resources {
	public:

						shared_resources(dfw::audio&, lda::resource_manager&, ldv::resource_manager&, lm::logger&, tools::chrono&, tools::arg_manager&);

#ifdef WDEBUG_CODE
	const tools::json_config_file&		get_debug_config() const {return debug_config;}
	void					reload_debug_config() {debug_config.reload();}
#endif

	dfw::audio&				get_audio() {return audio;}
	const lda::resource_manager&		get_audio_resource_manager() const {return a_manager;}
	const ldtools::ttf_manager&		get_ttf_manager() const {return ttf_manager;}
	const ldv::resource_manager&		get_video_resource_manager() const {return v_manager;}
	lm::logger&				get_log() {return log;}
	const ldtools::sprite_table&		get_tilesheet(size_t i) const {return tilesheets[i];}
	const ldtools::animation_table&		get_animation(size_t i) const {return animations[i];}
	tools::chrono&				get_controller_chrono() {return controller_chrono;}
	const tools::arg_manager&		get_arg_manager() const {return arg_manager;}

	private:

	void					register_fonts();
	void					register_tilesets();
	void					register_animations();

	//References
	dfw::audio				audio;
	lda::resource_manager&			a_manager;
	ldv::resource_manager&			v_manager;
	lm::logger&					log;
	tools::chrono&				controller_chrono;
	tools::arg_manager&			arg_manager;

	//Properties
	ldtools::ttf_manager			ttf_manager;
	std::vector<ldtools::sprite_table>	tilesheets;
	std::vector<ldtools::animation_table>	animations;

#ifdef WDEBUG_CODE
	tools::json_config_file			debug_config;
#endif
};

}

