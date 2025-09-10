#include "controller/_template.h"
#include "app/input.h"

using namespace controller;

_template::_template(
	app::service_provider _sp
):
	log{_sp.get_logger()}
{}

void _template::loop(
	dfw::input& _input, 
	const dfw::loop_iteration_data& /*_lid*/
) {

	if(_input().is_exit_signal() || _input.is_input_down(app::escape)) {
		set_leave(true);
		return;
	}
}

void _template::draw(
	ldv::screen& _screen, 
	int /*fps*/
) {

	_screen.clear(ldv::rgba8(0, 0, 0, 255));
}
