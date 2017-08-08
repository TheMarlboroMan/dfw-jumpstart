#ifndef DRAW_STRUCT_H
#define DRAW_STRUCT_H

#include <def_video.h>

//Lazy drawing solution... This class has an item of each kind of common 
//representation (minus ttf). Instead of creating and discarding representations
//for each drawable, this structure can be passed and its representations
//manipulated.

//It has lots of convenience methods, but being a struct, everything is free
//and accessible.

namespace app
{

struct draw_struct
{
	///////////////////////////////////
	// Defs

	enum class types{
		bitmap,
		group,
		points,
		box,
		line,
		polygon,
		external
	};

	///////////////////////////////////
	// Public interface

								draw_struct();
								~draw_struct();

	//////////
	//Generics
	void							go_to(ldv::point p) {rep->go_to(p);}
	ldv::rect 						get_view_position() const {return rep->get_view_position();}
	void 							set_type(types t);
	void 							set_alpha(unsigned int v);
	void 							set_blend(ldv::representation::blends);
	void							set_color(ldv::rgb_color);
	void 							set_rotation_center(float x, float y);
	void 							set_rotation(float v) {rep->set_rotation(v);}
	void							set_external(ldv::representation * prep) {set_external(*prep);}
	void							set_external(ldv::representation&);

	///////////////////
	//Group representation manipulation...
	void 							insert_in_group(ldv::representation * r);

	////////////////////
	//Point manipulation...
	void 							insert_point(ldv::point p) {rep_points.insert(p);}

	////////////////////
	//Primitive generics...
	void							set_primitive_fill(ldv::polygon_representation::type f);

	////////////////////
	//Manipulation of box representation...
	void							set_box_dimensions(int x, int y) {rep_box.set_dimensions(x,y);}
	void							set_box_location(const ldv::rect& r) {rep_box.set_location(r);}

	////////////////////
	//Manipulation of lines...
	void 							set_line(ldv::point p1, ldv::point p2) {rep_line.set_points(p1.x, p1.y, p2.x, p2.y);}

	////////////////////
	//Polygon manipulation...
	void							set_polygon_points(const std::vector<ldv::point>& p) {rep_polygon.set_points(p);}


	////////////////////
	//Métodos para manipular la representación de caja.

	void 							set_location_box(ldv::rect p) {rep_box.set_location(p);}

	///////////////////
	// Métodos para manipular la representación de bitmap...

	void 							set_clip(ldv::rect r) {rep_bmp.set_clip(r);}
	void 							set_location(ldv::rect r) {rep_bmp.set_location(r);}
	void 							set_set_invert_horizontal_bitmap(bool v) {rep_bmp.set_invert_horizontal(v);}
	void 							set_set_invert_vertical_bitmap(bool v) {rep_bmp.set_invert_vertical(v);}
	void 							set_texture(const ldv::texture& r) {rep_bmp.set_texture(r);}

	///////////////////////////////////
	// Propiedades

	ldv::bitmap_representation 			rep_bmp;
	ldv::group_representation 			rep_group;
	ldv::point_representation 			rep_points;
	ldv::box_representation 			rep_box;
	ldv::line_representation 			rep_line;
	ldv::polygon_representation 			rep_polygon;
	ldv::representation * 				rep;
	types 						type;
};

}

#endif
