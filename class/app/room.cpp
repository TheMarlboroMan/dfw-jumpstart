#include "room.h"

//Std
#include <algorithm>

//Local
#include "display_defs.h"

//Tools
#include <class/dnot_parser.h>
#include <templates/compatibility_patches.h>

using namespace app;

room::room()
{

}

room_drawable_collection room::get_drawables() const
{
	room_drawable_collection res;

	//The order of insertion goes "floor" then "shadows".
	for(const auto& ft: floor_tiles) 
		res.background.push_back(&ft);

	for(const auto& st: shadow_tiles) 
		res.background.push_back(&st);

	//Next game objects...

	//Finally, foreground, if any.

	return res;
}

//Loads a room from the file indicated by fn. fn is only the name, like map01.dat,
//without the full path.

void room::load(const std::string& fn)
{
	try
	{
		//This is non transactional. Any failure and we drop it.
		clear();

		std::string filename="data/app_data/maps/"+fn;
		auto root=tools::dnot_parse_file(filename);

		//First, layers...
		const auto& layers=root["data"]["layers"].get_vector();

		//First item is the logic layer.
		//TODO:

		//Lambda to push tiles...
		auto push_tile=[](const tools::dnot_token& i, std::vector<tile_decoration>& tiles, int tset_id, int res_id, int alpha)
		{
			int x=i["x"], y=i["y"], id=i["t"];
			tiles.push_back(
				tile_decoration(x, y, id, tset_id, res_id, alpha));
		};

		//Second item is the background.
		if(layers.size() >= 2)
			for(auto& i : layers[1]["data"].get_vector())
				push_tile(i, floor_tiles, tileset_defs::background, texture_defs::bgtiles, bgtiles_alpha);

		//Third item is the shadow layer.
		if(layers.size() >= 3)
			for(auto& i : layers[2]["data"].get_vector())
				push_tile(i, shadow_tiles, tileset_defs::shadows, texture_defs::shadowtiles, shadowtiles_alpha);

		//Next, object layers...
		const auto& logic=root["data"]["logic"].get_vector();

		//First layer is a lot of room objects...
		if(logic.size() >= 1)
		{
			for(const auto& i: logic[0]["data"].get_vector())
				build_room_object(i);
		}
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("Unable to load "+fn+": "+e.what());
	}
}

//This is sort of a factory... The kind of thing we could delegate to another
//class...

void room::build_room_object(const tools::dnot_token& tok)
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
			case 2: //Exit
				exits.push_back({
					app_interfaces::spatiable::t_box(tok["x"].get_int(), tok["y"].get_int(), tok["w"].get_int(), tok["h"].get_int()),
					tok["p"]["destination_map"],
					ifs(tok["p"]["terminus_id"])
					});
			break;
		}
	}
	catch(std::exception& e)
	{
		throw std::runtime_error(std::string("Unable to build object of type ")+compat::to_string(tok["t"].get_int())+std::string(":")+e.what());
	}
}

//Empties the room and basically leaves its state as after constructed.

void room::clear()
{
	//TODO: Clear all the other things.

	entrances.clear();
	exits.clear();
	floor_tiles.clear();
	shadow_tiles.clear();

}

const room_entrance& room::get_entrance_by_id(int id) const
{
	auto res=std::find_if(std::begin(entrances), std::end(entrances),
		[id](const room_entrance& re) {return re.get_terminus_id()==id;});

	if(res==std::end(entrances))
	{
		throw std::runtime_error("Unable to find terminus id "+compat::to_string(id));
	}

	return *res;
}
