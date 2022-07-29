#include "../../include/controller/test_2d_text.h"

//local
#include "../../include/input/input.h"

#include <lm/log.h>

#include <ldv/ttf_representation.h>

#include <tools/json.h>
#include <tools/file_utils.h>

//std
#include <cassert>


using namespace controller;

test_2d_text::test_2d_text(app::shared_resources& sr, dfw::signal_dispatcher& sd)
try
	:s_resources(sr),
	receiver(sd),
	state(tstates::dictate),
	text_buffer{{"This is the elusive text you will never see unless\nyou run this controller directly."}, {"Congratulations. You saw it."}, {"Now, please, move along."}},
	current_index(0),
	time_text(0.f),
	time_blink(0.f) {

	setup_signal_receiver();
	layout.map_font("main_text_font", s_resources.get_ttf_manager().get("consola-mono", 16));

	auto document=tools::parse_json_string(
		tools::dump_file("data/app_data/layouts.json")
	);

	layout.parse(document["text_bubble_layout"]);
}
catch(std::exception& e) {

	lm::log(sr.get_log()).error()<<"Unable to create 2d text controller: "<<e.what()<<std::endl;
	//This would still propagate: initialization lists and exceptions work like that.
}

void test_2d_text::loop(dfw::input& input, const dfw::loop_iteration_data& lid) {

	if(input().is_exit_signal()) {

		set_leave(true);
		return;
	}
	else if(input.is_input_down(input::escape)) {
		set_state(state_menu);
		return;
	}

	switch(state) {

		case tstates::dictate:

			lm::log(s_resources.get_log()).debug()<<"into dictate state"<<std::endl;

			if(input.is_input_down(input::activate)) {
				current_index=std::begin(text_buffer)->size();
				redraw_text();
			}
			else {

				time_text+=lid.delta;
				if(time_text >= 0.05f) {

					++current_index;
					if(current_index >= std::begin(text_buffer)->size()) {

						current_index=std::begin(text_buffer)->size();
						state=tstates::wait;
						time_blink=0.f;
					}

					time_text=0.f;
					redraw_text();
				}
			}

		break;

		case tstates::wait:

			lm::log(s_resources.get_log()).debug()<<"into wait state"<<std::endl;

			time_blink+=lid.delta;

			if(input.is_input_down(input::activate)) {

				text_buffer.erase(std::begin(text_buffer));
				if(!text_buffer.size()) {

					lm::log(s_resources.get_log()).debug()<<"end of buffer"<<std::endl;

					set_state(state_test_2d);
				}
				else {

					lm::log(s_resources.get_log()).debug()<<"back to dictate state"<<std::endl;

					time_text=0.f;
					current_index=0;
					state=tstates::dictate;
					redraw_text();
				}
			}
		break;
	}
}

void test_2d_text::draw(ldv::screen& screen, int /*fps*/) {

	if(state==tstates::wait) {

		bool visible=fmod(time_blink, 2.f) >= 0.5f;
		layout.get_by_id("wait_square")->set_visible(visible);
		layout.get_by_id("wait_triangle")->set_visible(visible);
	}
	else {

		layout.get_by_id("wait_square")->set_visible(false);
		layout.get_by_id("wait_triangle")->set_visible(false);
	}

	layout.draw(screen);
}

void test_2d_text::request_draw(dfw::controller_view_manager& cvm) {

	cvm.add(state_test_2d);
	cvm.add_ptr(this);
}

void test_2d_text::receive(const dfw::broadcast_signal& signal) {

	lm::log(s_resources.get_log()).debug()<<"got signal into test_2d_test: "<<std::endl;

	switch(signal.get_type()) {

		case t_signal_text_display:

			lm::log(s_resources.get_log()).debug()<<"it is a text display signal"<<std::endl;
			text_buffer=tools::explode(static_cast<const signal_text_display&>(signal).text, "[@np]");

			lm::log(s_resources.get_log()).debug()<<"text is '"<<static_cast<const signal_text_display&>(signal).text<<"'"<<std::endl;

			current_index=0;
			state=tstates::dictate;
			time_blink=0.f;
			time_text=0.f;
			redraw_text();
		break;
		case t_signal_reset_state:

			lm::log(s_resources.get_log()).debug()<<"it is a text reset signal"<<std::endl;

			text_buffer.clear();
			current_index=0;
			state=tstates::dictate;
			time_blink=0.f;
			time_text=0.f;
		break;
	}
}

void test_2d_text::redraw_text() {
	static_cast<ldv::ttf_representation *>(layout.get_by_id("main_text"))->set_text(std::begin(text_buffer)->substr(0, current_index));
}

//Forwards...
void test_2d_text::setup_signal_receiver() {
	receiver.f=[this](const dfw::broadcast_signal& s) {receive(s);};
}
