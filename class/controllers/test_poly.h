#ifndef CONTROLLER_TEST_POLY_H
#define CONTROLLER_TEST_POLY_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//local
#include "states.h"
#include "signals.h"

#include "../app/shared_resources.h"

namespace app
{

class controller_test_poly:
	public dfw::controller_interface
{
	public:

						controller_test_poly(shared_resources&, dfw::signal_dispatcher&);
	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input&, float);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input&) {}
	virtual void 				slumber(dfw::input&) {}
	virtual bool				can_leave_state() const {return true;}

	private:

	//A few useful structs...
	struct obstacle
	{
		ldt::polygon_2d<double>	poly;
		ldv::rgb_color		color;
	};

	struct waypoint
	{
		ldt::polygon_2d<double>	poly;
		int 			index;
	};

	struct player
	{
		ldt::polygon_2d<double>	poly;
		double			bearing,
					speed;
	};

	//references...
	shared_resources&			s_resources;
		
	//properties
	ldv::camera				camera;
	std::vector<obstacle>			obstacles;

	//Editor trash...
	typedef ldt::point_2d<int>		editor_pt;

	void 					editor_loop(dfw::input&);
	void					editor_draw(ldv::screen&);
	void					editor_draw_grid(ldv::screen&);
	void					editor_draw_vertex(ldv::screen&, ldt::point_2d<int>);
	void					editor_draw_current_poly(ldv::screen&);
	editor_pt				editor_cursor_position(bool=true);
	void					editor_close_poly();
	void					editor_save();
	void					editor_load();
	void					editor_select_color(int);

	static const int			editor_grid_size=64;
	ldi::sdl_input::mouse_position		editor_mouse_position;
	std::vector<editor_pt>			editor_current_poly;
	std::vector<ldv::rgb_color>		editor_colors;
	int					editor_color_index;
	ldv::box_representation 		editor_vertex_rep;
};

}

#endif
