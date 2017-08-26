#include "test_2d_text.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test_2d_text::controller_test_2d_text(shared_resources& sr)
	:s_resources(sr), 
	text_rep{
		s_resources.get_ttf_manager().get("consola-mono", 12), 
		ldv::rgba8(255, 255, 255, 255), 
		""},
	state(tstates::dictate), current_index(0),
	time_text(0.f), time_blink(0.f)
{

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
				if(time_text >= 0.1f)
				{
					++current_index;
					if(current_index >= std::begin(text_buffer)->size())
					{
						current_index=std::begin(text_buffer)->size();
						state=tstates::wait;
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
	//TODO: This might as well be on the composition.
	ldv::box_representation box{
		ldv::polygon_representation::type::fill, 
		{0,0,screen.get_w(), screen.get_h()}, 
		ldv::rgba8(0,0,128,128)
	};
	box.set_blend(ldv::representation::blends::alpha);

	//TODO: If state is wait, we display the blinking crap. Use ceil and mod to wrap to seconds.

	box.draw(screen);

	//TODO: Even better, get the text from the composition.
	text_rep.draw(screen);
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
	//TODO: Even better, get the text from the composition.
	text_rep.set_text(std::begin(text_buffer)->substr(0, current_index));
}
