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

namespace app
{

struct room_drawable_collection
{
	//Long live raw pointers. Semantically it means "this memory is not yours".
	std::vector<const app_interfaces::drawable *>	background; //These are not ordered.
	std::vector<const app_interfaces::drawable *>	game;	//The must be ordered.
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
	std::vector<const app_interfaces::spatiable *>	get_walls_by_box(const app_interfaces::spatiable::t_box&) const;
	
	private:

	static const int		bgtiles_alpha=255,
					shadowtiles_alpha=128;

	void				clear();
	void				build_room_object(const tools::dnot_token&);

	std::vector<tile_decoration>	floor_tiles;
	std::vector<tile_decoration>	shadow_tiles;

	//Logic
	tools::matrix_2d<room_wall>	walls;
	std::vector<room_entrance>	entrances;
	std::vector<room_exit>		exits;

	//TODO: Logic objects: switches...
	//TODO: Logic objects: game obstacles.
};

}

#endif
