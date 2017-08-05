#ifndef TILE_DECORATION_H
#define TILE_DECORATION_H

#include "drawable.h"

namespace app
{

class tile_decoration:
	public app_interfaces::drawable
{
	//////////////
	//drawable

	public:

	virtual float		get_ordering_x() const;
	virtual float		get_ordering_y() const;
	virtual bool		is_in_camera(const ldv::rect&) const;

	//////////////
	//public class interface.
	
	public:

	tile_decoration(int x, int y, int tile_id, int tileset_resource_id, int alpha);

	private:

	static const int	cell_size=32;

	int			x,
				y,		//This is in game space... Grid space is useless now.
				tile_id,
				tileset_resource_id,
				alpha;
	ldv::rect		drawable_box;
};

}

#endif
