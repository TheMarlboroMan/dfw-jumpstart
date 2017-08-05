#ifndef CONTROLLER_TEST_H
#define CONTROLLER_TEST_H

//std
#include <cmath>

//libdansdl2
#include <def_video.h>

//framework
#include <class/controller_interface.h>

//Tools.
#include <class/ttf_manager.h>

//local
#include "states.h"

/*
class game_tile:
	public spatiable,
	public drawable
{
	//This should simply represent the game state, would go like:
	x, y (in grid... maybe not), type (indicating what it does).
};

};

class room:
{
	public:

	//This would need some methods like get drawables and such.
	//When it comes to drawables we must think of a struct like:

	drawable_struct
	{
		std::vector<const drawable const *>	drawables;
		enum class torder_types{none, object} 	ordering;
	}
	
	//So we can collect them separatedly and decice which ones we are going
	//to order (hint, we are not ordering any tiles, just objects).
	//TODO: The big dilemma here is if we have various calls to get_drawables
	//like in get_drawable_foreground() or something like that... I particularly
	//like the following solution because it is easy and clearly represents the
	//intention:

	drawable_struct
	{
		std::vector<const drawable const *>	drawables_tile; //These are not ordered.
		std::vector<const drawable *>		drawables_game;	//The must be ordered.
		std::vector<const drawable const *>	drawables_foreground; //These are not ordered.
	}; 

	//This way we would have no further need of a torder_types and we might as well 
	//return the drawables_game ordered.

	//Todo: Given that this is a particular projection, our regular drawable
	//interface does not suffice. We need to draw left to right and taking into top to bottom, ordering the y axis through the lower part of the box.
	//so our drawables would need some get_order_data()... Maybe it is enough to use the Y coordinate only.

	private:

	std::vector<game_tile>		logic_tiles;
	std::vector<decoration_tile>	floor_tiles;
	std::vector<decoration_tile>	shadow_tiles;

	
}


*/

namespace app
{

class controller_test:
	public dfw::controller_interface
{
	public:

						controller_test(ldt::log&, ldv::resource_manager&, const tools::ttf_manager&);

	virtual void 				preloop(dfw::input&, float, int) {}
	virtual void 				loop(dfw::input& input, float delta);
	virtual void 				postloop(dfw::input&, float, int) {}
	virtual void 				draw(ldv::screen& screen, int);
	virtual void 				awake(dfw::input& input);
	virtual void 				slumber(dfw::input& input);
	virtual bool				can_leave_state() const;

	private:

	//references...
	ldt::log&					log;
	const tools::ttf_manager&			ttf_man;

	//properties
};

}

#endif
