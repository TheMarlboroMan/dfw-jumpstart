#include "_template.h"

//local
#include "../input.h"

using namespace app;

controller_example::controller_example(tools::log& plog)
	:log(plog) {

}

void controller_example::loop(dfw::input& input, const dfw::loop_iteration_data& /*lid*/) {

	if(input().is_exit_signal() || input.is_input_down(input_app::escape)) {
		set_leave(true);
		return;
	}
}

void controller_example::draw(ldv::screen& screen, int /*fps*/) {

	screen.clear(ldv::rgba8(0, 255, 255, 255));
}
