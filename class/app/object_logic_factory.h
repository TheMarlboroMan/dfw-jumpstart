#ifndef OBJECT_LOGIC_FACTORY_H
#define OBJECT_LOGIC_FACTORY_H

//std
#include <vector>

//tools
#include <class/dnot_parser.h>

//local
#include "object_trigger.h"
#include "room_entrance.h"

namespace app
{

class object_logic_factory
{
	public:

					object_logic_factory(std::vector<room_entrance>&, std::vector<object_trigger>&);
	void				make_object(const tools::dnot_token&);

	private:

	std::vector<room_entrance>&	entrances;
	std::vector<object_trigger>&	triggers;
};


}

#endif
