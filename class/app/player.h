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

namespace app
{

class player:
	public app_interfaces::spatiable,
	public app_interfaces::drawable
{
	public:

	//////////////////////////
	//Public interface

					player();
	void				step(float);
	void				set_input(game_input);
	void				integrate_motion(float, motion::axis);
	void				cancel_movement(motion::axis);
	void				adjust_collision(const spatiable&, motion::axis);

	//////////////////////////
	//Spatiable implementation

	virtual app_interfaces::spatiable::t_box get_box() const {return bounding_box;}
	virtual void 			set_box_x(tpos v) {bounding_box.origin.x=v;}
	virtual void 			set_box_y(tpos v) {bounding_box.origin.y=v;}
	virtual void 			displace_box(tpos x, tpos y) {bounding_box.origin.x+=x; bounding_box.origin.y+=y;}
	virtual void 			set_box_w(tdim v) {bounding_box.w=v;}
	virtual void 			set_box_h(tdim v) {bounding_box.h=v;}

	//////////////////////////
	//Drawable implementation

	virtual tpos			get_ordering_x() const {return get_spatiable_x();}
	virtual tpos			get_ordering_y() const {return get_spatiable_ey();}
	virtual bool			is_in_camera(const ldv::rect&) const;
	virtual void			draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	static const int w=24;
	static const int h=14;
	static const int speed=140;

	int				choose_animation_frame() const;

	app_interfaces::spatiable::t_box		bounding_box,
//TODO: Do we really need this or can we just trace the vector back?.
							prev_bounding_box;
	bearing						player_bearing;

	//This controls the vector and integration.
	motion						motion_data;
	float						walk_time;
};

}

#endif
