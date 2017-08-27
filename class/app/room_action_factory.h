#ifndef ROOM_ACTION_FACTORY_H
#define ROOM_ACTION_FACTORY_H

//std
#include <vector>
#include <memory>
#include <string>

//tools
#include <class/dnot_parser.h>

//local
#include "room_action.h"

namespace app
{

class room_action_factory
{
	public:

			room_action_factory(std::vector<std::unique_ptr<room_action>>&);
	void		make_action(const tools::dnot_token&);

	private:
	std::vector<std::unique_ptr<room_action>>&	actions;
};

} //end namespace

#endif
