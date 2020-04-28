#pragma once

//std
#include <vector>
#include <memory>
#include <string>

//tools
#include <rapidjson/document.h>

//local
#include "room_action.h"

namespace app
{

class room_action_factory
{
	public:

			room_action_factory(std::vector<std::unique_ptr<room_action>>&);
	void		make_action(const rapidjson::Value&);

	private:
	std::vector<std::unique_ptr<room_action>>&	actions;
};

} //end namespace

