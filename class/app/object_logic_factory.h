#ifndef OBJECT_LOGIC_FACTORY_H
#define OBJECT_LOGIC_FACTORY_H

//std
#include <vector>

//tools
#include <class/dnot_parser.h>

//local
#include "object_trigger.h"
#include "room_connection.h"

namespace app
{

class object_logic_factory
{
	public:

					object_logic_factory(std::vector<room_entrance>&, std::vector<room_exit>&, std::vector<object_trigger>&);
	void				make_object(const tools::dnot_token&);

	private:

	std::vector<room_entrance>&	entrances;
	std::vector<room_exit>&		exits;
	std::vector<object_trigger>&	triggers;
};


}

#endif
