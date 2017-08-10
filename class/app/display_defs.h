#ifndef DISPLAY_DEFS_H
#define DISPLAY_DEFS_H

namespace app
{

//Textures...
struct texture_defs
{
	enum {bgtiles=1, 
		shadowtiles=2,
		playersheet=3};
};

//This is the index for a vector.
struct tileset_defs
{
	enum {background=0,
		shadows=1};
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
