#include "../../include/app/object_logic_factory.h"

//std
#include <stdexcept>

using namespace app;

object_logic_factory::object_logic_factory(
	std::vector<room_entrance>& ent,
	std::vector<object_trigger>& trg,
	std::vector<object_audio_player>& aup
)
	:entrances(ent), triggers(trg), audio_players(aup)
{

}

void object_logic_factory::make_object(const rapidjson::Value& tok)
{
	try
	{
		//Caution: all properties are expressed as strings in the map format.
		auto ifs=[](const rapidjson::Value& t) -> int
		{
			std::string str=t.GetString();
			return std::atoi(str.c_str());
		};

		switch(tok["t"].GetInt())
		{
//TODO: push_back makes gratuitous copies everywhere...
			case 1: //Entrance
				entrances.push_back({
					tpoint(tok["x"].GetInt(), tok["y"].GetInt()),
					ifs(tok["p"]["bearing"]),
					ifs(tok["p"]["terminus_id"])
					});

			break;
			case 2: //Audio...
				audio_players.push_back({
					tpoint(tok["x"].GetInt(), tok["y"].GetInt()),
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
					tbox(tok["x"].GetInt(), tok["y"].GetInt(), tok["w"].GetInt(), tok["h"].GetInt()),
					object_trigger::ttype::touch,
					ifs(tok["p"]["action_id"])
					});
			break;
			case 4: //Action trigger...
				triggers.push_back({
					tbox(tok["x"].GetInt(), tok["y"].GetInt(), tok["w"].GetInt(), tok["h"].GetInt()),
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
		throw std::runtime_error(std::string("Unable to build object of type ")+std::to_string(tok["t"].GetInt())+std::string(":")+e.what());
	}
}
