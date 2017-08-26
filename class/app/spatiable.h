#ifndef SPATIABLE_H
#define SPATIABLE_H

#include <libDan2.h>
#include "app_defs.h"

namespace app_interfaces
{

class spatiable
{
	// Definitions.

	public:

	typedef ldt::box<tpos, tdim>		t_box;
	typedef ldt::polygon_2d<tpos>	 	t_poly;
	typedef ldt::point_2d<tpos> 		t_point;

	// Public interface.

	public:

	virtual ~spatiable() {}

	bool 				is_colliding_with(const spatiable&) const;
	bool 				is_colliding_with(const t_poly&) const;

	void				move_by(t_point);
	void				center_on(t_point);
	void				center_on(const spatiable& s) {center_on(s.get_poly());}
	void				center_on(t_poly);

	//To implement.

	virtual const t_poly&		get_poly() const=0;
	//A pointer, just in case we can't return a reference... We can always go nullptr.
	virtual t_poly *		get_poly_ptr()=0;
};

//These get a spatiable bounding box from a polygon or spatiable.
spatiable::t_box	coarse_bounding_box(const spatiable&);
spatiable::t_box	coarse_bounding_box(const spatiable::t_poly&);

}

#endif
