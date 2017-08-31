#include "object_decoration_factory.h"

#include <templates/compatibility_patches.h>

using namespace app;

object_decoration_factory::object_decoration_factory()
{
	auto t=tools::dnot_parse_file("data/app_data/decoration-data.dat");
	data=t["data"].get_map();
}

object_decoration object_decoration_factory::make_object(const tools::dnot_token& t)
{
	try
	{
		//This is in "t" {p:{}, t:5, x:517, y:352}

		//This is in "info" {w:27, h:17, frame_index: 1},
		const auto& info=data["type_"+compat::to_string(t["t"].get_int())];

		tpos 	x=t["x"].get_int(), 
			y=t["y"].get_int(), 
			w=info["w"].get_int(), 
			h=info["h"].get_int();

		tpoly poly{
			{ {x,y}, {x+w,y}, {x+w,y+h}, {x,y+h} },
			{x, y}};

		return object_decoration(poly, info["frame_index"].get_int());
	}
	catch(std::exception &e)
	{
		std::string msg="Unable to make object:";
		throw std::runtime_error(msg+e.what());
	}
}
