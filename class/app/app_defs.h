#pragma once

//tools
#include <ldt/vector_2d.h>

//libdan2
#include <ldt/box.h>
#include <ldt/polygon_2d.h>


//Basic application typedefs... 

namespace app
{

//Represents the type for a point in space.
typedef double					tpos;

//Represents the type for the length of any object in space.
typedef unsigned int				tdim ;

//Represents the type of movement vector.
typedef ldt::vector_2d<tpos> 		tvector;

//Types for spatiable data...
typedef ldt::box<tpos, tdim>			tbox;
typedef ldt::polygon_2d<tpos>	 		tpoly;
typedef ldt::point_2d<tpos> 			tpoint;

}
