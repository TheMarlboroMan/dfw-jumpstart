#ifndef SPATIABLE_H
#define SPATIABLE_H

/*Interfaz que exige los métodos para definir una caja en 2d y a partir de ahí,
monta el resto. Que defina una caja 2d no significa que realmente haya una
caja por ahí, por ejemplo, las celdas de un nivel no tienen cajas reales, aunque
devuelven una caja calculada. 

Está implícito también que implementa muchas funciones para temas de colisión,
como por ejemplo, saber si es sólido por todos lados o no, obtener información
de colisión y demás. Intentamos separarlo en dos interfaces pero teníamos tanto
el problema del diamante como que son casi la misma cosa. Se quedan, por tanto,
como están.
*/

#include <libDan2.h>

namespace app_interfaces
{

class spatiable
{
	///////////////////////////////
	// Definiciones

	public:

	typedef ldt::box<float, unsigned int> 	t_box;
	typedef ldt::point_2d<float> 		t_point;
	enum t{T_X=1, T_Y=2};

	///////////////////////////////
	// Propiedades e internas...

	protected:

	//////////////////////////////
	// Interfaz pública.

	public:

	virtual ~spatiable() {}

	//x, y, center x and y, end x and y.
	float 				get_spatiable_x() const {return get_box().origin.x;}
	float 				get_spatiable_y() const {return get_box().origin.y;}
	float 				get_spatiable_cx() const {return get_spatiable_x() + (get_spatiable_w() / 2);}
	float 				get_spatiable_cy() const {return get_spatiable_y() + (get_spatiable_h() / 2);}
	float 				get_spatiable_ex() const {return get_spatiable_x() + get_spatiable_w();}
	float 				get_spatiable_ey() const {return get_spatiable_y() + get_spatiable_h();}

	
	unsigned int 			get_spatiable_w() const {return get_box().w;}
	unsigned int 			get_spatiable_h() const {return get_box().h;}
	t_point		 		get_spatiable_position() const {return get_box().origin;}

	ldt::vector_2d_screen<float>	screen_vector_for(const spatiable& e) const {return screen_vector_for(*this, e);}
	ldt::vector_2d_screen<float>	screen_vector_for(const spatiable& a, const spatiable& b) const;
	ldt::vector_2d_cartesian<float>	cartesian_vector_for(const spatiable& e) const {return cartesian_vector_for(*this, e);}
	ldt::vector_2d_cartesian<float>	cartesian_vector_for(const spatiable& a, const spatiable& b) const;

	float		 		angle_for(const spatiable& e) const {return angle_for(*this, e);}
	float		 		angle_for(const spatiable& a, const spatiable& b) const;
	float		 		cartesian_angle_for(const spatiable& e) const {return cartesian_angle_for(*this, e);}
	float		 		cartesian_angle_for(const spatiable& a, const spatiable& b) const;

	bool 				is_colliding_with(const spatiable&, bool=false) const;
	bool 				is_colliding_with(const t_box& e, bool=false) const;

	//Estos son siempre según la geometría en la que 0.0 es el punto superior
	//a la izquierda.

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
	t_box 				get_box_displaced(float x, float y) const;

	////////////////////////////
	// A implementar

	virtual t_box 			get_box() const=0;	//Debe devolver una COPIA de la caja.

	virtual void 			set_box_x(float)=0;	//Estos no tienen porqué hacer nada si no hay una caja real.
	virtual void 			set_box_y(float)=0;
	virtual void 			displace_box(float, float)=0;	//Este desplazaría x e y unidades.
	virtual void 			set_box_w(unsigned int)=0;
	virtual void 			set_box_h(unsigned int)=0;
};

}

#endif
