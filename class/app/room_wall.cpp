#include "room_wall.h"

using namespace app;

room_wall::room_wall(int cellx, int celly)
	:bounding_box(cellx*wall_w, celly*wall_h, wall_w, wall_h)
{

}
