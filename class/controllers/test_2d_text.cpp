#include "test_2d_text.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test_2d_text::controller_test_2d_text(shared_resources& sr)
try
	:s_resources(sr), 
	state(tstates::dictate), current_index(0),
	time_text(0.f), time_blink(0.f)
{
	layout.map_font("main_text_font", s_resources.get_ttf_manager().get("consola-mono", 16));
	layout.parse("data/app_data/layouts.dat", "text_bubble_layout");
}
catch(std::exception& e)
{
	sr.get_log()<<"Unable to create 2d text controller: "<<e.what()<<std::endl;
	//This would still propagate: initialization lists and exceptions work like that.
}

void controller_test_2d_text::loop(dfw::input& input, float delta)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{ 
		set_leave(true);
		return;
	}

	switch(state)
	{
		case tstates::dictate:

			if(input.is_input_down(input_app::activate))
			{
				current_index=std::begin(text_buffer)->size();
				redraw_text();
			}
			else
			{
				time_text+=delta;
				if(time_text >= 0.05f)
				{
					++current_index;
					if(current_index >= std::begin(text_buffer)->size())
					{
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

			time_blink+=delta;

			if(input.is_input_down(input_app::activate))
			{
				text_buffer.erase(std::begin(text_buffer));
				if(!text_buffer.size())
				{
					set_state(state_test_2d);
				}
				else
				{
					time_text=0.f;
					current_index=0;
					state=tstates::dictate;
					redraw_text();
				}
			}
		break;
	}
}

void controller_test_2d_text::draw(ldv::screen& screen, int /*fps*/)
{
	if(state==tstates::wait)
	{
		bool visible=fmod(time_blink, 2.f) >= 0.5f;
		layout.get_by_id("wait_square")->set_visible(visible);
		layout.get_by_id("wait_triangle")->set_visible(visible);
	}
	else
	{
		layout.get_by_id("wait_square")->set_visible(false);
		layout.get_by_id("wait_triangle")->set_visible(false);
	}

	layout.draw(screen);
}

void controller_test_2d_text::request_draw(dfw::controller_view_manager& cvm)
{
	cvm.add(state_test_2d); 
	cvm.add_ptr(this);
}

void controller_test_2d_text::receive(const dfw::broadcast_message& msg)
{
	if(msg.type==0)
	{
		text_buffer=tools::explode(msg.tok["data"]["txt"], "[@np]");
	}
}

void controller_test_2d_text::redraw_text()
{
	static_cast<ldv::ttf_representation *>(layout.get_by_id("main_text"))->set_text(std::begin(text_buffer)->substr(0, current_index));
}
