#pragma once

//App
#include "tile_decoration.h"
#include "room_entrance.h"
#include "room_wall.h"
#include "object_decoration.h"
#include "object_trigger.h"
#include "room_action.h"
#include "object_audio_player.h"

//Tools
#include <tools/dnot_token.h>
#include <tools/matrix_2d.h>


//Std
#include <vector>
#include <string>


namespace app
{

struct room_drawable_collection
{
	//Long live raw pointers. Semantically it means "this memory is not yours".
	std::vector<const app_interfaces::drawable *>	background; //These are not ordered.
	std::vector<const app_interfaces::drawable *>	main;	//The must be ordered.
	std::vector<const app_interfaces::drawable *>	foreground; //These are not ordered.
};

struct room_music_data
{
	size_t			music_id;
	unsigned int		ms_fade_in;
	unsigned int		ms_fade_out;
	unsigned int		volume;
};

class room
{
	public:

					room();

	room_drawable_collection	get_drawables() const; 	//This should use RVO, so no problem with superfluous copies.
	void				load(const std::string&);
	const room_entrance&		get_entrance_by_id(int) const;
	const 	std::vector<object_trigger>&	get_triggers() const {return triggers;}
	std::vector<const app_interfaces::spatiable *>	get_walls_by_box(const tbox&) const;
	std::vector<const app_interfaces::spatiable *> get_obstacles() const;
	std::vector<object_audio_player>&	get_audio_players() {return audio_players;}
	void				inject_audio_dispatcher(app_interfaces::channel_dispatcher_interface&);
	unsigned int			get_w() const {return walls.get_w()*room_wall::wall_w;}
	unsigned int			get_h() const {return walls.get_h()*room_wall::wall_h;}
	object_trigger *		get_trigger_memory() {return trigger_memory.get();}
	void				clear_trigger_memory() {trigger_memory.reset(nullptr);}
	void				set_trigger_memory(const object_trigger& o) {trigger_memory.reset(new object_trigger{o});}
	const room_action *		get_action(int) const;
	const room_music_data&		get_music_data() const {return music_data;}

#ifdef WDEBUG_CODE
	const tools::matrix_2d<room_wall>& get_all_walls() {return walls;} 
#endif
	
	private:

	//Constants...
	static const int		bgtiles_alpha=255,
					shadowtiles_alpha=128;

	//Private methods...
	void				clear();

	//Properties.

	std::vector<tile_decoration>	floor_tiles;
	std::vector<tile_decoration>	shadow_tiles;
	room_music_data			music_data;

	//Logic
	std::unique_ptr<object_trigger>	trigger_memory; //copy of the last touch trigger touched.
	tools::matrix_2d<room_wall>	walls;
	std::vector<room_entrance>	entrances;
	std::vector<object_decoration>	decorations;
	std::vector<object_trigger>	triggers;
	std::vector<object_audio_player>	audio_players;
	std::vector<std::unique_ptr<room_action>>	actions;
};

}

