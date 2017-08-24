#include "object_decoration.h"

#include "display_defs.h"

using namespace app;

object_decoration::object_decoration(const app_interfaces::spatiable::t_poly& tp, int frame)
	:poly(tp), frame_index(frame)
{

}

bool object_decoration::is_in_camera(const ldv::rect& /*r*/) const
{
	//TODO: 
	return true;
}

void object_decoration::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& sr) const
{
	const auto& frame=sr.get_tilesheet(tileset_defs::decoration).get(frame_index);
	const auto& frect=frame.get_rect();

	ds.set_type(app::draw_struct::types::bitmap);
	ds.set_texture(sr.get_video_resource_manager().get_texture(texture_defs::decoration_object_sheet));
	ds.set_clip(frect);
	ds.set_location(
		ds.location_projection(
			ds.drawable_box_from_box_polygon(poly), 
			frect, frame.disp_x, frame.disp_y));
	ds.rep->draw(scr, cam);
}
