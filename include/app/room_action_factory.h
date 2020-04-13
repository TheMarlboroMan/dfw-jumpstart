#pragma once

//std
#include <vector>
#include <memory>
#include <string>

//tools
#include <tools/dnot_parser.h>

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

