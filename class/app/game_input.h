#ifndef GAME_INPUT_H
#define GAME_INPUT_H

namespace app
{

//A structure that saves player input so it can be known by the player
//and the controller.

struct game_input
{
	game_input()
		:x(0), y(0)
	{}

	int x, y;
};

}

#endif
