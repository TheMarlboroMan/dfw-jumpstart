#include "object_logic_factory.h"

//std
#include <stdexcept>

//tools
#include <templates/compatibility_patches.h>

using namespace app;

object_logic_factory::object_logic_factory(
	std::vector<room_entrance>& ent, 
	std::vector<object_trigger>& trg,
	std::vector<object_audio_player>& aup
)
	:entrances(ent), triggers(trg), audio_players(aup)
{

}

void object_logic_factory::make_object(const tools::dnot_token& tok)
{
	try
	{
		//Caution: all properties are expressed as strings in the map format.
		auto ifs=[](const tools::dnot_token& t) -> int
		{
			return std::atoi(t.get_string().c_str());
		};

		switch(tok["t"].get_int())
		{
//TODO: push_back makes gratuitous copies everywhere...
			case 1: //Entrance
				entrances.push_back({
					app_interfaces::spatiable::t_point(tok["x"].get_int(), tok["y"].get_int()),
					ifs(tok["p"]["bearing"]),
					ifs(tok["p"]["terminus_id"])
					});

			break;
			case 2: //Audio...
				audio_players.push_back({
					app_interfaces::spatiable::t_point(tok["x"].get_int(), tok["y"].get_int()),
					ifs(tok["p"]["type"])==1 ? object_audio_player::ttype::ambient : object_audio_player::ttype::source,
					{		//object_audio_player_data  
						ifs(tok["p"]["sound_id"]),
						ifs(tok["p"]["volume"]),
						ifs(tok["p"]["repeat"]),
						ifs(tok["p"]["pre_delay_min"]),
						ifs(tok["p"]["pre_delay_random"]),
						0.f, //Remaining time...
						(tpos)ifs(tok["p"]["max_radius_source"]),
						(tpos)ifs(tok["p"]["min_radius_source"])
					}
				});
			break;
			case 3: //Touch trigger...
				triggers.push_back({
					app_interfaces::spatiable::t_box(tok["x"].get_int(), tok["y"].get_int(), tok["w"].get_int(), tok["h"].get_int()),
					object_trigger::ttype::touch,
					ifs(tok["p"]["action_id"])
					});
			break;
			case 4: //Action trigger...
				triggers.push_back({
					app_interfaces::spatiable::t_box(tok["x"].get_int(), tok["y"].get_int(), tok["w"].get_int(), tok["h"].get_int()),
					object_trigger::ttype::activate,
					ifs(tok["p"]["action_id"])
					});
			break;
			default:
				throw std::runtime_error("Unknown object type");
			break;
		}
	}
	catch(std::exception& e)
	{
		throw std::runtime_error(std::string("Unable to build object of type ")+compat::to_string(tok["t"].get_int())+std::string(":")+e.what());
	}
}
