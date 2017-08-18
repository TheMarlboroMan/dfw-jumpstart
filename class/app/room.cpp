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
	:walls{1,1}
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

		//Lambda to push decoration tiles...
		auto push_tile=[](const tools::dnot_token& i, std::vector<tile_decoration>& tiles, int tset_id, int res_id, int alpha)
		{
			int x=i["x"], y=i["y"], id=i["t"];
			tiles.push_back(
				tile_decoration(x, y, id, tset_id, res_id, alpha));
		};

		//First item is the logic layer.
		if(layers.size() >= 1)
		{
			walls.clear();
			walls.resize(layers[0]["info"]["w"].get_int(), layers[0]["info"]["h"].get_int());

			for(auto& i : layers[0]["data"].get_vector())
				walls.insert(i["x"].get_int(), i["y"].get_int(), room_wall(i["x"].get_int(), i["y"].get_int(), i["t"].get_int()));
		}

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

std::vector<const app_interfaces::spatiable *>	room::get_walls_by_box(const app_interfaces::spatiable::t_box& box) const
{
	std::vector<const app_interfaces::spatiable *> res;


	//Converts a world value to a cell value. The last parameter is used
	//in edge cases where the "end" of a box aligns with the beginning of
	//a cell, thus including the cell in the result. It also does not allow
	//negative values...

	//TODO: Protect against right and botto limits. walls.get_w() & walls.get_h()

	

	auto to_cell=[](int v, int dim, bool check_remainder)
	{
		int val=floor(v/dim);
		if(check_remainder && !(v%dim)) --val;

		return val >= 0 ? val : 0;
	};

	//The ceil part makes sense: when passed to to_cell, box.origin.x+box.w
	//is parsed to int, which just removes its decimal part, meaning that
	//if you have an end in 32.7 it becomes 32 (thus decremented by "to_cell")
	//instead of 33. The beg_X values are not subjected to the floor 
	//treatment, as this is done by default.

	size_t 	beg_x=to_cell(box.origin.x, room_wall::wall_w, false),
		end_x=to_cell(std::ceil(box.origin.x+box.w), room_wall::wall_w, true), //adjusting...
		beg_y=to_cell(box.origin.y, room_wall::wall_h, false),
		end_y=to_cell(std::ceil(box.origin.y+box.h), room_wall::wall_h, true);

	for(size_t x=beg_x; x <= end_x; x++)
	{
		for(size_t y=beg_y; y <= end_y; y++)
		{
			if(walls.check(x, y))
			{
				res.push_back(&walls(x, y));
			}
		}
	}

	return res;
}
