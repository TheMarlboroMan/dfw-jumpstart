#ifndef CONTROLLER_TEST_POLY_H
#define CONTROLLER_TEST_POLY_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//tools
#include <templates/ranged_value.h>

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
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
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
		ldt::vector_2d<double>	velocity,
					thrust;
		double			bearing;
		int			health;
	};

	struct stimer
	{
		int			ms=0, seconds=0, minutes=0;
		void			reset() {ms=0; seconds=0; minutes=0;}
		void			step(double d)
		{
			ms+=d*1000.;
			if(ms >= 1000) 
			{	
				ms=0; ++seconds;
				if(seconds >= 60)
				{
					seconds=0; ++minutes;
				}
			}
		}
					operator bool() const {return minutes || seconds || ms;}
		bool 			operator<(const stimer& o) const
		{
			if(minutes!=o.minutes) return minutes < o.minutes;
			else if(seconds!=o.seconds) return seconds < o.seconds;
			else return ms < o.ms;
		}
	};

	//Methods

	void					load();
	void					soft_reset();
	void					reset();
	void					draw_polygon(ldv::screen&, const ldt::polygon_2d<double>&, ldv::rgb_color, int);
	void					draw_raster(ldv::screen&, ldv::raster_representation&);
	void					draw_hud(ldv::screen&);

	void					do_camera(float);
	void					do_waypoint();

	void					player_accelerate(float, double);
	void					player_idle(float);
	void					player_turn(float, int);
	void					player_step(float);

#ifdef WDEBUG_CODE
	//Debug
	void					reload_physics_values();
	ldt::point_2d<double>			debug_collision_line_pt1, 
						debug_collision_line_pt2,
						debug_collision_normal_pt1,
						debug_collision_normal_pt2;

	//Editor trash...
	typedef ldt::point_2d<int>		editor_pt;

	void 					editor_loop(dfw::input&);
	void					editor_draw(ldv::screen&);
	void					editor_draw_grid(ldv::screen&);
	void					editor_draw_vertex(ldv::screen&, ldt::point_2d<int>, ldv::rgb_color);
	void					editor_draw_line(ldv::screen&, ldt::point_2d<int>, ldt::point_2d<int>, ldv::rgb_color);
	void					editor_draw_current_poly(ldv::screen&);
	editor_pt				editor_cursor_position(bool=true);
	void					editor_close_poly();
	void					editor_save();
	void					editor_select_color(int);
	void					editor_select_waypoint(int);
	void					editor_change_state();
	template<typename T> void		editor_delete_item(std::vector<T>& v,  ldt::point_2d<double> point)
	{
		auto it=std::remove_if(std::begin(v), std::end(v), [point](const T& o) {return ldt::point_in_polygon<double>(o.poly, point);});
		if(it!=std::end(v)) v.erase(it);
	}
#endif

//references...
	shared_resources&			s_resources;

	//consts... not so much in debug mode :P.
#ifdef WDEBUG_CODE
#define const /* */
#endif

	const int				max_health=3;
	const double				min_zoom,
						max_zoom,
						max_zoom_velocity,
						player_full_stop_threshold,
						player_acceleration,
						player_base_turn_factor,
						player_max_turn_velocity,
						player_max_thrust,
						player_min_thrust,
						friction_coeficient;

#ifdef WDEBUG_CODE
#undef const
#endif
		
	//properties
	enum class tstates{intro, play, game_over} state;
	ldv::camera				camera;
	stimer					timer,
						best_time;
	player					player;
	ldv::rgb_color				player_color;
	std::vector<obstacle>			obstacles;
	std::vector<waypoint>			waypoints;
	struct	{
		double				current,
						next;
	}					camera_val;
	struct  {
		int				current=0,
						total=0;
	}					waypoint_val;

#ifdef WDEBUG_CODE
	//Editor properties.

	bool					editor_active=true;
	static const int			editor_grid_size=64;
	enum class 				editor_modes{obstacles, waypoints} editor_mode;
	ldi::sdl_input::mouse_position		editor_mouse_position;
	std::vector<editor_pt>			editor_current_poly;
	std::vector<ldv::rgb_color>		editor_colors;
	int					editor_color_index=0,
						editor_waypoint_index=1;
	ldv::box_representation 		editor_vertex_rep;
	ldv::line_representation 		editor_line_rep;
#endif
};

}

#endif
