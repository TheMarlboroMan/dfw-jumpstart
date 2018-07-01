#ifndef APP_DEFS_H
#define APP_DEFS_H

//libdan2

#include <tools/box/box.h>
#include <tools/polygon_2d/polygon_2d.h>

//tools
#include <templates/vector_2d_screen.h>

//Basic application typedefs... 

namespace app
{

//Represents the type for a point in space.
typedef double					tpos;

//Represents the type for the length of any object in space.
typedef unsigned int				tdim ;

//Represents the type of movement vector.
typedef tools::vector_2d_screen<tpos> 		tvector;

//Types for spatiable data...
typedef ldt::box<tpos, tdim>			tbox;
typedef ldt::polygon_2d<tpos>	 		tpoly;
typedef ldt::point_2d<tpos> 			tpoint;

}
#endif
