#ifndef SPATIABLE_H
#define SPATIABLE_H

#include <libDan2.h>
#include "app_defs.h"

namespace app_interfaces
{

class spatiable
{
	///////////////////////////////
	// Definiciones

	public:

	typedef ldt::box<tpos, tdim>	 	t_box;
	typedef ldt::point_2d<tpos> 		t_point;

	// Interfaz pública.

	public:

	virtual ~spatiable() {}

	//x, y, center x and y, end x and y.
	tpos 				get_spatiable_x() const {return get_box().origin.x;}
	tpos 				get_spatiable_y() const {return get_box().origin.y;}
	tpos 				get_spatiable_cx() const {return get_spatiable_x() + (get_spatiable_w() / 2);}
	tpos 				get_spatiable_cy() const {return get_spatiable_y() + (get_spatiable_h() / 2);}
	tpos 				get_spatiable_ex() const {return get_spatiable_x() + get_spatiable_w();}
	tpos 				get_spatiable_ey() const {return get_spatiable_y() + get_spatiable_h();}

	tdim	 			get_spatiable_w() const {return get_box().w;}
	tdim	 			get_spatiable_h() const {return get_box().h;}
	t_point		 		get_spatiable_position() const {return get_box().origin;}

/*
	ldt::vector_2d_cartesian<tpos>	cartesian_vector_for(const spatiable& e) const {return cartesian_vector_for(*this, e);}
	ldt::vector_2d_cartesian<tpos>	cartesian_vector_for(const spatiable& a, const spatiable& b) const;

	tpos		 		angle_for(const spatiable& e) const {return angle_for(*this, e);}
	tpos		 		angle_for(const spatiable& a, const spatiable& b) const;
	tpos		 		cartesian_angle_for(const spatiable& e) const {return cartesian_angle_for(*this, e);}
	tpos		 		cartesian_angle_for(const spatiable& a, const spatiable& b) const;
*/

	bool 				is_colliding_with(const spatiable&, bool=false) const;
	bool 				is_colliding_with(const t_box& e, bool=false) const;

	//Estos son siempre según la geometría en la que 0.0 es el punto superior
	//a la izquierda... Any overlap would be "is_inside_of", so we need to
	//test positions previous to the overlap.

	bool 				is_over(const spatiable& otra) const {return is_over(get_box(), otra.get_box());}
	bool 				is_under(const spatiable& otra) const {return is_under(get_box(), otra.get_box());}
	bool 				is_left_of(const spatiable& otra) const {return is_left_of(get_box(), otra.get_box());}
	bool 				is_right_of(const spatiable& otra) const {return is_right_of(get_box(), otra.get_box());}
	bool				is_inside_of(const spatiable& otra) const {return is_inside_of(get_box(), otra.get_box());}

	bool 				is_over(const t_box& otra) const {return is_over(get_box(), otra);}
	bool 				is_under(const t_box& otra) const {return is_under(get_box(), otra);}
	bool 				is_left_of(const t_box& otra) const {return is_left_of(get_box(), otra);}
	bool 				is_right_of(const t_box& otra) const {return is_right_of(get_box(), otra);}
	bool				is_inside_of(const t_box& otra) const {return is_inside_of(get_box(), otra);}

	static bool 			is_over(const t_box& una, const t_box& otra) {return una.origin.y+una.h <= otra.origin.y;}
	static bool			is_under(const t_box& una, const t_box& otra) {return una.origin.y >= otra.origin.y+otra.h;}
	static bool			is_left_of(const t_box& una, const t_box& otra) {return una.origin.x+una.w <= otra.origin.x;}
	static bool			is_right_of(const t_box& una, const t_box& otra) {return una.origin.x >= otra.origin.x+otra.w;}
	static bool			is_inside_of(const t_box& una, const t_box& otra) {return una.inside(otra);}

	void				center_on(const spatiable&);
	void				center_on(t_point);
	void 				set_position(const spatiable& e);
	void 				set_position(t_point);
	t_box 				get_box_displaced(tpos x, tpos y) const;

	////////////////////////////
	// A implementar

	virtual t_box 			get_box() const=0;	//Debe devolver una COPIA de la caja.

	virtual void 			set_box_x(tpos)=0;	//Estos no tienen porqué hacer nada si no hay una caja real.
	virtual void 			set_box_y(tpos)=0;
	virtual void 			displace_box(tpos, tpos)=0;	//Este desplazaría x e y unidades.
	virtual void 			set_box_w(tdim)=0;
	virtual void 			set_box_h(tdim)=0;
};

}

#endif
