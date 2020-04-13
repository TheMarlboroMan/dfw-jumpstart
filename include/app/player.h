#pragma once

//Local
#include "motion.h"
#include "spatiable.h"
#include "drawable.h"
#include "game_defs.h"
#include "game_input.h"
#include "aurable.h"

//Dlib
#include <ldt/vector_2d.h>

namespace app
{

class player:
	public app::spatiable,
	public app::drawable,
	public app::aurable
{
	public:

	//////////////////////////
	//Public interface

						player();
	void					step(float);
	void					set_input(game_input);
	void					start_movement_phase();
	void					confirm_movement_stage();
	void					solve_movement_phase();
	void					integrate_motion(float, motion::axis);
	void					cancel_movement(motion::axis);
	tpoly					get_activate_poly() const;

	//////////////////////////
	//Spatiable implementation

	virtual const tpoly&			get_poly() const {return polygon;}
	virtual tpoly *				get_poly_ptr() {return &polygon;}

	//////////////////////////
	//Drawable implementation

	virtual tpos			get_ordering_x() const {return polygon.get_vertices()[0].x;}
	virtual tpos			get_ordering_y() const {return polygon.get_vertices()[2].y;}
	virtual bool			is_in_camera(const ldv::rect&) const;
	virtual void			draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	static const int w=24;
	static const int h=14;
	static const int speed=140;

	int				choose_animation_frame() const;

	tpoly						polygon,
							prev_polygon;
	tpoint						prev_position;
	bearing						player_bearing;

	//This controls the vector and integration.
	motion						motion_data;
	float						walk_distance,
							walk_time;
	const int					walk_step_distance=40;
	std::vector<size_t>				step_sounds;
	int						step_sounds_index;
};

}
