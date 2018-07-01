#ifndef MOTION_H
#define MOTION_H

//This would usually be an "interface" but let's try some composition instead.

#include <tools/vector_2d/vector_2d.h>

#include "app_defs.h"

namespace app
{

class motion {

	//////////////////////
	// Definiciones...
	public:

	enum class axis {x,y};

	///////////////////////
	// Public interface
	public:

						motion(tvector={0.f, 0.f});

	bool					has_motion() const {return vector.x || vector.y;}
	const tvector&			 	get_vector() const {return vector;}
	tvector		 			get_vector() {return vector;}

	tpos					get_vector(axis);
	tpos 					get_vector_x() const {return vector.x;}
	tpos 					get_vector_y() const {return vector.y;}

	void 					add_vector(tpos, axis);
	void 					set_vector(tpos, axis);
	void 					set_vector(tvector v) {vector=v;}
	tpos& 					ref_vector_x() {return vector.x;}
	tpos& 					ref_vector_y() {return vector.y;}

	//////////////////////
	// Properties...
	private:

	tvector					vector;

};

}

#endif
