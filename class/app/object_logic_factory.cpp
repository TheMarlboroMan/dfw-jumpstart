#include "object_logic_factory.h"

//std
#include <stdexcept>

//tools
#include <templates/compatibility_patches.h>

using namespace app;

object_logic_factory::object_logic_factory(std::vector<room_entrance>& ent, std::vector<object_trigger>& trg)
	:entrances(ent), triggers(trg)
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
			case 1: //Entrance
				entrances.push_back({
					app_interfaces::spatiable::t_point(tok["x"].get_int(), tok["y"].get_int()),
					ifs(tok["p"]["bearing"]),
					ifs(tok["p"]["terminus_id"])
					});

			break;
			case 2:
				//????
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
