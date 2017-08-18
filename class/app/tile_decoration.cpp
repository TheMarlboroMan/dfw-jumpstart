#include "tile_decoration.h"

using namespace app;

//px and py are in cell space...
tile_decoration::tile_decoration(int px, int py, int ptile_id, int ptileset_id, int ptileset_resource_id, int palpha)
	:x(px*cell_size), y(py*cell_size), 
	tile_id(ptile_id), tileset_id(ptileset_id), 
	tileset_resource_id(ptileset_resource_id),
	alpha(palpha), drawable_box{{x, y}, cell_size, cell_size}
{

}

bool tile_decoration::is_in_camera(const ldv::rect& c_rect) const
{
	return drawable_box.collides_with(c_rect);
}

void tile_decoration::draw(ldv::screen& scr, const ldv::camera& cam, app::draw_struct& ds, const app::shared_resources& sr) const
{
	ds.set_type(app::draw_struct::types::bitmap);
	ds.set_texture(sr.get_video_resource_manager().get_texture(tileset_resource_id));
	ds.set_alpha(alpha);
	ds.set_clip(sr.get_tilesheet(tileset_id).get(tile_id).get_rect());
	ds.set_location({x, y, cell_size, cell_size});
	ds.rep->draw(scr, cam);
}
