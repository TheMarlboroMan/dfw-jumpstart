#ifndef APP_DEFS_H
#define APP_DEFS_H

#include <templates/vector_2d_screen.h>

//Basic application typedefs... 

//Represents the type for a point in space.
typedef double		tpos;

//Represents the type for the length of any object in space.
typedef unsigned int	tdim ;

//Represents the type of movement vector.
typedef tools::vector_2d_screen<tpos> 		t_vector;

#endif
