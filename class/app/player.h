#ifndef PLAYER_H
#define PLAYER_H

//Dlib
#include <tools/vector_2d/vector_2d.h>

//Local
#include "motion.h"
#include "spatiable.h"
#include "drawable.h"
#include "game_defs.h"

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
	//TODO: We'll see...
//	void				set_bearing(game_defs::tbearing b) {bearing=b;}
	motion&				get_motion() {return motion_data;}

	//////////////////////////
	//Spatiable implementation

	virtual app_interfaces::spatiable::t_box get_box() const {return bounding_box;}
	virtual void 			set_box_x(float v) {bounding_box.origin.x=v;}
	virtual void 			set_box_y(float v) {bounding_box.origin.y=v;}
	virtual void 			displace_box(float x, float y) {bounding_box.origin.x+=x; bounding_box.origin.y+=y;}
	virtual void 			set_box_w(unsigned int v) {bounding_box.w=v;}
	virtual void 			set_box_h(unsigned int v) {bounding_box.h=v;}

	//////////////////////////
	//Drawable implementation

	virtual float			get_ordering_x() const {return get_spatiable_x();}
	virtual float			get_ordering_y() const {return get_spatiable_ey();}
	virtual bool			is_in_camera(const ldv::rect&) const;
	virtual void			draw(ldv::screen&, const ldv::camera&, app::draw_struct&, const app::shared_resources&) const;

	private:

	static const int w=24;
	static const int h=24;
	static const int speed=80;

	app_interfaces::spatiable::t_box		bounding_box,
							prev_bounding_box;
	bearing						player_bearing;

	//This controls the vector and integration.
	motion						motion_data;
};

}

#endif
