#pragma once

//Local
#include "object_decoration.h"

#include <rapidjson/document.h>

namespace app
{
class object_decoration_factory
{
	public:

					object_decoration_factory();
	object_decoration		make_object(const rapidjson::Value&);

	private:

	struct data_entry {
		int w, h, frame_index;
	};

	std::map<std::string, data_entry>	data;

};
}
