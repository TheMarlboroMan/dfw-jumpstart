#pragma once

//local
#include "object_trigger.h"
#include "room_entrance.h"
#include "object_audio_player.h"

//tools
#include <tools/dnot_parser.h>


//std
#include <vector>

namespace app
{

class object_logic_factory
{
	public:

					object_logic_factory(
						std::vector<room_entrance>&, 
						std::vector<object_trigger>&,
						std::vector<object_audio_player>&);
	void				make_object(const tools::dnot_token&);

	private:

	std::vector<room_entrance>&		entrances;
	std::vector<object_trigger>&		triggers;
	std::vector<object_audio_player>&	audio_players;
};


}
