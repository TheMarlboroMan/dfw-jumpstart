#ifndef ROOM_H
#define ROOM_H

//Std
#include <vector>

//App
#include "tile_decoration.h"

namespace app
{

struct room_drawable_collection
{
	//Long live raw pointers. Semantically it means "this memory is not yours".
	std::vector<const app_interfaces::drawable const *>	drawables_tile; //These are not ordered.
	std::vector<const app_interfaces::drawable *>		drawables_game;	//The must be ordered.
	std::vector<const app_interfaces::drawable const *>	drawables_foreground; //These are not ordered.
}; 

class room
{
	public:

	//This should use RVO, so no problem with superfluous copies.
	room_drawable_collection	get_drawables() const;

	private:

	//TODO: We'll probably need the filename.
	//TODO: We'll probably need a "load" function.
	//TODO: We'll also need a clean function.
	//TODO: We'll need collision functions.

//	std::vector<game_tile>		logic_tiles;
	std::vector<tile_decoration>	floor_tiles;
	std::vector<tile_decoration>	shadow_tiles;
};

}

#endif
