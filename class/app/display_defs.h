#ifndef DISPLAY_DEFS_H
#define DISPLAY_DEFS_H

namespace app
{

//Index textures for resource manager.
struct texture_defs
{
	enum {bg_tiles=1, 
		shadow_tiles=2,
		player_sheet=3,
		decoration_object_sheet=4};
};

//This is the index for a vector of sheets.
struct tileset_defs
{
	enum {background=0,
		shadows=1,
		player,
		decoration};
};

//This is the index for a vector.
struct animation_defs
{
	enum {player=0
	};
};

struct player_animation_defs
{
	enum {
		idle_n=1,
		walk_n,
		idle_e,
		walk_e,
		idle_s,
		walk_s,
		idle_w,
		walk_w};
};

}

#endif
