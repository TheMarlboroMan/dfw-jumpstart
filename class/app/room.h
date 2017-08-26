#ifndef ROOM_H
#define ROOM_H

//Std
#include <vector>
#include <string>

//Tools
#include <class/dnot_token.h>
#include <templates/matrix_2d.h>

//App
#include "tile_decoration.h"
#include "room_connection.h"
#include "room_wall.h"
#include "object_decoration.h"
#include "object_trigger.h"

namespace app
{

struct room_drawable_collection
{
	//Long live raw pointers. Semantically it means "this memory is not yours".
	std::vector<const app_interfaces::drawable *>	background; //These are not ordered.
	std::vector<const app_interfaces::drawable *>	main;	//The must be ordered.
	std::vector<const app_interfaces::drawable *>	foreground; //These are not ordered.
}; 

class room
{
	public:

					room();

	room_drawable_collection	get_drawables() const; 	//This should use RVO, so no problem with superfluous copies.
	void				load(const std::string&);
	const room_entrance&		get_entrance_by_id(int) const;
	const std::vector<room_exit>&	get_exits() const {return exits;}
	const 	std::vector<object_trigger>&	get_triggers() const {return triggers;}
	std::vector<const app_interfaces::spatiable *>	get_walls_by_box(const app_interfaces::spatiable::t_box&) const;
	std::vector<const app_interfaces::spatiable *> get_obstacles() const;
	unsigned int			get_w() const {return walls.get_w()*room_wall::wall_w;}
	unsigned int			get_h() const {return walls.get_h()*room_wall::wall_h;}
	object_trigger *		get_trigger_memory() {return trigger_memory.get();}
	void				clear_trigger_memory() {trigger_memory.reset(nullptr);}
	void				set_trigger_memory(const object_trigger& o) {trigger_memory.reset(new object_trigger{o});}

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

	//Logic
	std::unique_ptr<object_trigger>	trigger_memory; //copy of the last touch trigger touched.
	tools::matrix_2d<room_wall>	walls;
	std::vector<room_entrance>	entrances;
	std::vector<room_exit>		exits;
	std::vector<object_decoration>	decorations;
	std::vector<object_trigger>	triggers;
};

}

#endif
