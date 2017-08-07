#include "room.h"

#include "texture_defs.h"

#include <class/dnot_parser.h>

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

		auto layers=root["data"]["layers"].get_vector();

		//First item is the logic sheet.
		//TODO:

		//Lambda to push tiles...
		auto push_tile=[](const tools::dnot_token& i, std::vector<tile_decoration>& tiles, int res_id, int alpha)
		{
			int x=i["x"], y=i["y"], id=i["t"];
			tiles.push_back(
				tile_decoration(x, y, id, res_id, alpha));
		};

		//Second item is the background.
		if(layers.size() >= 2)
			for(auto& i : layers[1]["data"].get_vector())
				push_tile(i, floor_tiles, texture_defs::bgtiles, bgtiles_alpha);

		//Third item is the shadow layer.
		if(layers.size() >= 3)
			for(auto& i : layers[2]["data"].get_vector())
				push_tile(i, shadow_tiles, texture_defs::shadowtiles, shadowtiles_alpha);
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("Unable to load "+fn+": "+e.what());
	}
}

//Empties the room and basically leaves its state as after constructed.

void room::clear()
{
	//TODO: Clear all the other things.

	floor_tiles.clear();
	shadow_tiles.clear();

}
