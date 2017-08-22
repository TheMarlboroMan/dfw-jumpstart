#include "room_connection.h"

using namespace app;

room_object::room_object(app_interfaces::spatiable::t_box b)
	:polygon{
		{ 
			{b.origin.x, b.origin.y}, 
			{b.origin.x+(tpos)b.w, b.origin.y}, 
			{b.origin.x+(tpos)b.w, b.origin.y+(tpos)b.h}, 
			{b.origin.x, b.origin.y+(tpos)b.h} },
		{b.origin.x+((tpos)b.w/2), b.origin.y+((tpos)b.h/2)}}
{

}
