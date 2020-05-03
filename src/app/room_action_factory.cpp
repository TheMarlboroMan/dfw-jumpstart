#include "../../include/app/room_action_factory.h"

#include <stdexcept>

using namespace app;

room_action_factory::room_action_factory(std::vector<std::unique_ptr<room_action>>& a)
	:actions(a) {

}

void room_action_factory::make_action(const rapidjson::Value& tok) {

	int 	id=tok["id"].GetInt();
	bool	repeat=tok["repeat"].GetBool();

	switch(tok["type"].GetInt()) {

		case 1: //Room exit
			actions.emplace_back(new room_action_exit(id, repeat, tok["room"].GetString(), tok["terminus_id"].GetInt()));
		break;
		case 2:	//Text trigger
			actions.emplace_back(new room_action_text(id, repeat, tok["text_id"].GetInt()));
		break;
		case 3: //Console
			actions.emplace_back(new room_action_console(id, repeat));
		break;
		case 4: //Arcade
			actions.emplace_back(new room_action_arcade(id, repeat));
		break;
		default:
			throw std::runtime_error("Unknown action type in factory");
		break;
	}
}
