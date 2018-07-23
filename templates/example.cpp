#include "example.h"

//local
#include "../input.h"

using namespace app;

controller_example::controller_example(tools::log& plog)
	:log(plog), r(0), g(255), b(255) {

}

void controller_example::loop(dfw::input& input, const dfw::loop_iteration_data& /*lid*/) {

	if(input().is_exit_signal() || input.is_input_down(input_app::escape)) {
		set_leave(true);
		return;
	}
	else if(input().is_event_input()) {

		r=(r+10) % 255;
		g=(g+20) % 255;
		b=(b+30) % 255;
	}
}

void controller_example::draw(ldv::screen& screen, int /*fps*/) {

	screen.clear(ldv::rgba8(0, r, g, b));
}
