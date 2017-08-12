#ifndef MOTION_H
#define MOTION_H

//This would usually be an "interface" but let's try some composition instead.

#include <tools/vector_2d/vector_2d.h>

namespace app
{

class motion
{
	//////////////////////
	// Definiciones...
	public:

	enum class axis {x,y};
	typedef ldt::vector_2d<float> 		t_vector;

	///////////////////////
	// Public interface
	public:

						motion(t_vector={0.f, 0.f});

	bool					has_motion() const {return vector.x || vector.y;}
	const t_vector&			 	get_vector() const {return vector;}
	t_vector		 		get_vector() {return vector;}

	float					get_vector(axis);
	float 					get_vector_x() const {return vector.x;}
	float 					get_vector_y() const {return vector.y;}

	void 					add_vector(float, axis);
	void 					set_vector(float, axis);
	void 					set_vector(t_vector v) {vector=v;}
	float& 					ref_vector_x() {return vector.x;}
	float& 					ref_vector_y() {return vector.y;}

	//////////////////////
	// Properties...
	private:

	t_vector				vector;

};

}

#endif
