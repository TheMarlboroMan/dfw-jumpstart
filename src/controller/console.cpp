#include "../../include/controller/console.h"

//local
#include "../../include/input/input.h"

//tools

//ld
#include <ldv/ttf_representation.h>
#include <ldv/box_representation.h>

//std
#include <cassert>
#include <sstream>

using namespace controller;

console::console(app::shared_resources& sr)
	:s_resources(sr),
	current_command(),
	bgc_r(32), bgc_g(32), bgc_b(32), fgc_r(255), fgc_g(255), fgc_b(255), time(0.f) {

}

void console::loop(dfw::input& input, const dfw::loop_iteration_data& lid) {

	if(input().is_exit_signal()) {
		set_leave(true);
		return;
	}
	else  if(input.is_input_down(input::escape)) {
		set_state(state_menu);
		return;
	}

	//TODO: Learn to use mod?
	time+=lid.delta;
	if(time > 1.f) time=0.f;

	//Keyboard control... Newline (enter) and backspace are controlled separatedly
	//from text and do not trigger text inputs.

	if(input().is_event_text()) {
		if(current_command.size() < 30) {
			current_command+=input().get_text_input();
		}
		input().clear_text_input();
	}
	else if(input.is_input_down(input::console_newline)) {
		//A history of commands is kept up to 10 lines.
		history.push_back(current_command);
		if(history.size() > 10) {
			history.erase(std::begin(history), std::begin(history)+1);
		}

		//This is pretty much stupid... A list of commands and their results.
		if(current_command=="clear") {
			history.clear();
		}
		else if(current_command=="help") {
			history.push_back("clear, bgcolor r g b, fgcolor r g b:, exit");
		}
		else if(current_command=="exit") {
			set_state(state_test_2d);
		}
		else if(current_command.substr(0, 7)=="bgcolor") {
			try {
				do_color_change(bgc_r, bgc_g, bgc_b);
			}
			catch(std::exception& e) {
				history.push_back("Syntax: bgcolor r g b "+std::string(e.what()));
			}
		}
		else if(current_command.substr(0, 7)=="fgcolor") {
			try {
				do_color_change(fgc_r, fgc_g, fgc_b);
			}
			catch(std::exception& e) {
				history.push_back("Syntax: fgcolor r g b "+std::string(e.what()));
			}
		}
		else {
			history.push_back("Syntax error: "+current_command+" not recognised. Try help");
		}

		current_command.clear();
	}
	else if(input.is_input_down(input::console_backspace)) {
		if(current_command.size()) {
			current_command.pop_back();
		}
	}
}

void console::draw(ldv::screen& screen, int /*fps*/) {
	screen.clear(ldv::rgba8(bgc_r, bgc_g, bgc_b, 0));

	std::string history_text;
	for(auto& s: history) {
		history_text+=s+"\n";
	}

	auto& font=s_resources.get_ttf_manager().get("consola-mono", 16);
	auto fgcolor=ldv::rgba8(fgc_r, fgc_g, fgc_b, 255);
	ldv::ttf_representation txt_history(font, fgcolor, history_text);
	txt_history.set_alpha(128);
	txt_history.draw(screen);

	ldv::ttf_representation txt_command(font, fgcolor, ">"+current_command);
	txt_command.go_to({0, 450});
	txt_command.draw(screen);

	if(time < 0.5f) {
		auto txt_box=txt_command.get_text_position();
		ldv::rect box_rect={txt_box.origin.x+(int)txt_box.w, txt_box.origin.y+(int)txt_box.h-16, 4, 16};
		ldv::box_representation box(box_rect, fgcolor);
		box.draw(screen);
	}
}

void console::awake(dfw::input& input) {
	input().start_text_input();
	input().clear_text_input();
}

void console::slumber(dfw::input& input) {
	input().stop_text_input();
}

bool console::can_leave_state() const {
	return true;
}

void console::do_color_change(int& r, int& g, int& b) {

	std::string type;
	std::stringstream ss{current_command};
	int pr{}, pg{}, pb{};

	ss>>type>>pr>>pg>>pb;
		
	if(ss.fail()) {
		throw std::runtime_error("invalid syntax");	
	}
		
	r=pr;
	g=pg;
	b=pb;
}
