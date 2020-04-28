#include "../../include/app/object_decoration_factory.h"

#include <tools/json.h>
#include <tools/file_utils.h>

#include <ldt/polygon_2d.h>

using namespace app;

object_decoration_factory::object_decoration_factory() {

	auto document=tools::parse_json_string(
		tools::dump_file(
			"data/app_data/decoration-data.json"
		)
	);

	for(const auto& entry : document.GetArray()) {

		std::string key=entry["id"].GetString();
		data[key]={
			entry["w"].GetInt(),
			entry["h"].GetInt(),
			entry["frame_index"].GetInt()
		};
	}
}

object_decoration object_decoration_factory::make_object(const rapidjson::Value& _t) {

	try {
		const auto& info=data["type_"+std::to_string(_t["t"].GetInt())];

		tpos 	x=_t["x"].GetInt(),
			y=_t["y"].GetInt(),
			w=info.w,
			h=info.h;

		tpoly poly{
			{ {x,y}, {x+w,y}, {x+w,y+h}, {x,y+h} },
			{x, y}};

		return object_decoration(poly, info.frame_index);
	}
	catch(std::exception &e) {
		std::string msg="Unable to make object:";
		throw std::runtime_error(msg+e.what());
	}
}
