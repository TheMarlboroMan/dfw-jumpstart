#ifndef OBJECT_DECORATION_H
#define OBJECT_DECORATION_H

//Tools
#include <class/sprite_table.h>

//Local
#include "spatiable.h"
#include "drawable.h"



namespace app
{
class object_decoration:
	public app_interfaces::spatiable,
	public app_interfaces::drawable
{
	public:

							object_decoration(const tpoly&, int);
	
	// Spatiable

	virtual const tpoly&	get_poly() const {return poly;}
	virtual tpoly *		get_poly_ptr() {return &poly;}
	
	// Drawable

	virtual tpos		get_ordering_x() const {return poly.get_vertices()[0].x;}
	virtual tpos		get_ordering_y() const {return poly.get_vertices()[2].y;}
	virtual bool		is_in_camera(const ldv::rect&) const;
	virtual void		draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	tpoly						poly;
	int						frame_index;

};
}

#endif
