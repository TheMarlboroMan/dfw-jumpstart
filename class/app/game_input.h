#pragma once

namespace app
{

//A structure that saves player input so it can be known by the player
//and the controller.

struct game_input
{
	int x=0, y=0;
	bool activate=false;
};

}
