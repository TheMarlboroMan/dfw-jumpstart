#include "tile_decoration.h"

using namespace app;

//px and py are in cell space...
tile_decoration::tile_decoration(int px, int py, int ptile_id, int ptileset_resource_id, int palpha)
	:x(px*cell_size), y(py*cell_size), tile_id(ptile_id), tileset_resource_id(ptileset_resource_id),
	alpha(palpha), drawable_box{{x, y}, cell_size, cell_size}
{

}

//These are not meant to be ordered, so there's no need to even try.

float tile_decoration::get_ordering_x() const
{
	return 0.f;
}

float tile_decoration::get_ordering_y() const
{
	return 0.f;
}

bool tile_decoration::is_in_camera(const ldv::rect& c_rect) const
{
	return drawable_box.collides_with(c_rect);
}
