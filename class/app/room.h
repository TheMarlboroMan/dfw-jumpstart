#ifndef ROOM_H
#define ROOM_H

//Std
#include <vector>
#include <string>

//App
#include "tile_decoration.h"

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

	//This should use RVO, so no problem with superfluous copies.
	room_drawable_collection	get_drawables() const;
	void				load(const std::string&);
	
	private:

	static const int		bgtiles_alpha=255,
					shadowtiles_alpha=128;

	void				clear();

	//TODO: We'll also need a clean function.
	//TODO: We'll need collision functions.

//	TODO: This gotta be a 2d matrix. std::vector<game_tile>		logic_tiles;
	std::vector<tile_decoration>	floor_tiles;
	std::vector<tile_decoration>	shadow_tiles;
};

}

#endif
