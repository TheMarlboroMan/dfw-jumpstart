#ifndef TILE_DECORATION_H
#define TILE_DECORATION_H

#include "drawable.h"

namespace app
{

class tile_decoration:
	public app_interfaces::drawable
{
	//////////////
	//public class interface.
	
	public:

	tile_decoration(int, int, int, int, int, int);


	//////////////
	//drawable

	public:

	virtual float		get_ordering_x() const;
	virtual float		get_ordering_y() const;
	virtual bool		is_in_camera(const ldv::rect&) const;
	virtual void		draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	static const int	cell_size=32;

	int			x,
				y,		//This is in game space... Grid space is useless now.
				tile_id,
				tileset_id,
				tileset_resource_id,
				alpha;
	ldv::rect		drawable_box;
};

}

#endif
