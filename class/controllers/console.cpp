#include "console.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>
#include <class/dnot_parser.h>

//local
#include "../input.h"

using namespace app;

controller_console::controller_console(ldt::log& plog, const tools::ttf_manager& pttf_man, const ldv::resource_manager& rm)
	:log(plog), ttf_man(pttf_man), res_man(rm),
	current_command(),
	bgc_r(32), bgc_g(32), bgc_b(32), fgc_r(255), fgc_g(255), fgc_b(255), time(0.f)
{

}

void controller_console::loop(dfw::input& input, float delta)
{
	time+=delta;
	if(time > 1.f) time=0.f;

	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	if(input().is_event_text())
	{
		if(input.is_input_down(input_app::console_newline))
		{
			current_command=input().get_text_input();
			current_command.pop_back();
			history.push_back(current_command);
			if(history.size() > 10) history.erase(std::begin(history), std::begin(history)+1);
	
			if(current_command=="clear") history.clear();
			else if(current_command=="help") history.push_back("clear, bgcolor:, fgcolor:, exit");
			else if(current_command=="exit") set_state(state_main);
			else if(current_command.substr(0, 8)=="bgcolor:") 
			{
				try {do_color_change("bgcolor", bgc_r, bgc_g, bgc_b);}
				catch(std::exception& e) {history.push_back("Syntax: bgcolor:[r,g,b] "+std::string(e.what()));}
			}
			else if(current_command.substr(0, 8)=="fgcolor:") 
			{
				try {do_color_change("fgcolor", fgc_r, fgc_g, fgc_b);}
				catch(std::exception& e) {history.push_back("Syntax: fgcolor:[r,g,b] "+std::string(e.what()));}
			}
			else 
			{
				history.push_back("Syntax error: "+current_command+" not recognised. Try help");
			}

			current_command="";
			input().clear_text_input(); 
		}
		else
		{
			current_command=input().get_text_input();
			if(current_command.size() > 30) 
			{
				current_command=current_command.substr(0, 30);
			}
		}
	}
}

void controller_console::draw(ldv::screen& screen)
{
	screen.clear(ldv::rgba8(bgc_r, bgc_g, bgc_b, 0));

	//A few stupid bitmaps.. Just to see how they work, with alpha, colorize and so on.
/*
	ldv::bitmap_representation img1(res_man.get_texture(2), {400,0,200,100}, {0,0,1000,665});
	img1.draw(screen);

	ldv::bitmap_representation img2(res_man.get_texture(2), {400,100,200,100}, {0,0,1000,665});
	img2.set_alpha(128);
	img2.set_blend(ldv::representation::blends::alpha);
	img2.draw(screen);

	ldv::bitmap_representation img3(res_man.get_texture(1), {400,200,200,100}, {0,0,456,376});
	img3.set_colorize(ldv::rgb8(255,0,0));
	img3.draw(screen);

	ldv::bitmap_representation img4(res_man.get_texture(1), {400,300,200,100}, {0,0,456,376});
	img4.set_colorize(ldv::rgb8(0,255,0));
	img4.set_alpha(128);
	img4.set_blend(ldv::representation::blends::alpha);
	img4.draw(screen);

	ldv::bitmap_representation img5(res_man.get_texture(1), {400,400,200,100}, {0,0,456,376});
	img5.set_colorize(ldv::rgb8(0,0,255));
	img5.set_alpha(128);
	img5.set_blend(ldv::representation::blends::alpha);
	img5.draw(screen);
*/

	std::string history_text;
	for(auto& s: history) history_text+=s+"\n";
	
	auto fgcolor=ldv::rgba8(fgc_r, fgc_g, fgc_b, 255);
	ldv::ttf_representation txt_history(ttf_man.get("consola-mono", 16), fgcolor, history_text);
	txt_history.set_alpha(128);
	txt_history.draw(screen);

	ldv::ttf_representation txt_command(ttf_man.get("consola-mono", 16), fgcolor, ">"+current_command);
	txt_command.go_to(0, 450);
	txt_command.draw(screen);
	
	if(time < 0.5f)
	{
		auto txt_box=txt_command.get_view_position();
		ldv::rect box_rect={txt_box.origin.x+(int)txt_box.w, txt_box.origin.y+(int)txt_box.h-16, 4, 16}; 
		ldv::box_representation box(ldv::polygon_representation::type::fill, box_rect, fgcolor);
		box.draw(screen);
	}
}

void controller_console::awake(dfw::input& input)
{
	input().start_text_input();
	input().clear_text_input();
}

void controller_console::slumber(dfw::input& input)
{
	input().stop_text_input();
}

bool controller_console::can_leave_state() const
{
	return true;
}

void controller_console::do_color_change(const std::string& key, int& r, int& g, int& b)
{
	auto tok=tools::dnot_parse_string(current_command);
	r=tok[key][0];
	g=tok[key][1];
	b=tok[key][2];
}
