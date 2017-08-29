#ifndef PLAYER_H
#define PLAYER_H

//Dlib
#include <tools/vector_2d/vector_2d.h>

//Local
#include "motion.h"
#include "spatiable.h"
#include "drawable.h"
#include "game_defs.h"
#include "game_input.h"
#include "aurable.h"

namespace app
{

class player:
	public app_interfaces::spatiable,
	public app_interfaces::drawable,
	public app_interfaces::aurable
{
	public:

	//////////////////////////
	//Public interface

						player();
	void					step(float, const app_interfaces::spatiable::t_box&);
	void					set_input(game_input);
	void					integrate_motion(float, motion::axis);
	void					cancel_movement(motion::axis);
	app_interfaces::spatiable::t_poly	get_activate_poly() const;

	//////////////////////////
	//Spatiable implementation

	virtual const t_poly&		get_poly() const {return polygon;}
	virtual t_poly *		get_poly_ptr() {return &polygon;}

	//////////////////////////
	//Drawable implementation

	virtual tpos			get_ordering_x() const {return polygon.get_vertexes()[0].x;}
	virtual tpos			get_ordering_y() const {return polygon.get_vertexes()[2].y;}
	virtual bool			is_in_camera(const ldv::rect&) const;
	virtual void			draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	static const int w=24;
	static const int h=14;
	static const int speed=140;

	int				choose_animation_frame() const;

	app_interfaces::spatiable::t_poly		polygon,
							prev_polygon;
	bearing						player_bearing;

	//This controls the vector and integration.
	motion						motion_data;
	float						walk_time,
							next_step_sound;
};

}

#endif
