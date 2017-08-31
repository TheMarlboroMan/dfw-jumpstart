#ifndef SPATIABLE_H
#define SPATIABLE_H

#include <libDan2.h>
#include "app_defs.h"

namespace app_interfaces
{

class spatiable
{
	// Public interface.

	public:

	virtual ~spatiable() {}

	bool 				is_colliding_with(const spatiable&) const;
	bool 				is_colliding_with(const app::tpoly&) const;

	void				move_by(app::tpoint);
	void				center_on(app::tpoint);
	void				center_on(const spatiable& s) {center_on(s.get_poly());}
	void				center_on(app::tpoly);

	//To implement.

	virtual const app::tpoly&		get_poly() const=0;
	//A pointer, just in case we can't return a reference... We can always go nullptr.
	virtual app::tpoly *			get_poly_ptr()=0;
};

//These get a spatiable bounding box from a polygon or spatiable.
app::tbox		coarse_bounding_box(const spatiable&);
app::tbox		coarse_bounding_box(const app::tpoly&);

}

#endif
