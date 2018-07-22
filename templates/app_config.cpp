#include "app_config.h"

using namespace app;

app_config::app_config(): base_config(get_file_path()) {
}


dfw::input_description app::input_description_from_config_token(const tools::dnot_token& tok) {

	if(!tok.is_vector()) {
		throw std::runtime_error("app::input_description_from_config_token, token is not vector");
	}
	const auto& v=tok.get_vector();

	if(v.size() != 3) throw std::runtime_error("app::input_description_from_config_token, vector has not size of 3");
	return dfw::input_description{input_description_type_from_int(v[0]), v[2], v[1]};
}

tools::dnot_token app::config_token_from_input_description(const dfw::input_description& id) {

	tools::dnot_token res;
	res.set(std::vector<tools::dnot_token>{ tools::dnot_token{input_description_int_from_type(id.type)}, tools::dnot_token{id.device}, tools::dnot_token{id.code}});
	return res;
}

dfw::input_description::types app::input_description_type_from_int(int v) {

	switch(v) {
		case 0: return dfw::input_description::types::keyboard; break;
		case 1: return dfw::input_description::types::mouse; break;
		case 2: return dfw::input_description::types::joystick; break;
	}

	return dfw::input_description::types::none;
}

int app::input_description_int_from_type(dfw::input_description::types t) {

	switch(t) {
		case dfw::input_description::types::keyboard: 	return 0; break;
		case dfw::input_description::types::mouse: 	return 1; break;
		case dfw::input_description::types::joystick: 	return 2; break;
		case dfw::input_description::types::none: 	return 3; break;
	}

	return 3;
}
