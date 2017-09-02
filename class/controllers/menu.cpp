#include "menu.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_menu::controller_menu(shared_resources& s)
	:s_resources(s), 
	horizontal(ldv::representation_aligment::h::none),
	vertical(ldv::representation_aligment::v::none)
	
{
}

void controller_menu::preloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_menu::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	if(input.is_input_pressed(input_app::activate))
	{
		if(input.is_input_down(input_app::up)) --margin_v;
		else if(input.is_input_down(input_app::down)) ++margin_v;
		else if(input.is_input_down(input_app::left)) --margin_h;
		else if(input.is_input_down(input_app::right)) ++margin_h;
	}
	else
	{
		using namespace ldv;

		if(input.is_input_down(input_app::up))
		{
			switch(horizontal)
			{
				case representation_aligment::h::none: 		horizontal=representation_aligment::h::outer_left; break;
				case representation_aligment::h::outer_left:	horizontal=representation_aligment::h::inner_left; break;
				case representation_aligment::h::inner_left:	horizontal=representation_aligment::h::center; break;
				case representation_aligment::h::center:	horizontal=representation_aligment::h::inner_right; break;
				case representation_aligment::h::inner_right:	horizontal=representation_aligment::h::outer_right; break;
				case representation_aligment::h::outer_right:	horizontal=representation_aligment::h::none; break;
			}
		}

		if(input.is_input_down(input_app::down))
		{
			switch(vertical)
			{
				case representation_aligment::v::none: 		vertical=representation_aligment::v::outer_top; break;
				case representation_aligment::v::outer_top:	vertical=representation_aligment::v::inner_top; break;
				case representation_aligment::v::inner_top:	vertical=representation_aligment::v::center; break;
				case representation_aligment::v::center:	vertical=representation_aligment::v::inner_bottom; break;
				case representation_aligment::v::inner_bottom:	vertical=representation_aligment::v::outer_bottom; break;
				case representation_aligment::v::outer_bottom:	vertical=representation_aligment::v::none; break;
			}
		}
	}
}

void controller_menu::postloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_menu::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 0, 0, 0));

	std::string text="";

	using namespace ldv;

	switch(horizontal)
	{
		case representation_aligment::h::none: 		text+="h: none"; break;
		case representation_aligment::h::outer_left:	text+="h: outer left"; break;
		case representation_aligment::h::inner_left:	text+="h: inner left"; break;
		case representation_aligment::h::center:	text+="h: center"; break;
		case representation_aligment::h::inner_right:	text+="h: inner right"; break;
		case representation_aligment::h::outer_right:	text+="h: outer right"; break;
	}

	text+="\n";

	switch(vertical)
	{
		case representation_aligment::v::none: 		text+="v: none"; break;
		case representation_aligment::v::outer_top:	text+="v: outer top"; break;
		case representation_aligment::v::inner_top:	text+="v: inner top"; break;
		case representation_aligment::v::center:	text+="v: center"; break;
		case representation_aligment::v::inner_bottom:	text+="v: inner bottom"; break;
		case representation_aligment::v::outer_bottom:	text+="v: outer bottom"; break;
	}

	text+="\n"+compat::to_string(margin_h)+","+compat::to_string(margin_v);
	
	ldv::box_representation	a{ldv::polygon_representation::type::fill, {100, 100, 500, 300}, rgba8(255,0,0,255)};
	ldv::box_representation	b{ldv::polygon_representation::type::fill, {200, 200, 100, 100}, rgba8(0,0,255,255)};

	b.align(a, representation_aligment{horizontal, vertical, margin_h, margin_v});

	a.draw(screen);
	b.draw(screen);

	ldv::ttf_representation text_rep{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(255, 255, 255, 255), text, 2.f, ldv::ttf_representation::text_align::center};

	text_rep.align(a, representation_aligment{horizontal, vertical, margin_h, margin_v});

	text_rep.draw(screen);
}

void controller_menu::awake(dfw::input& /*input*/)
{

}

void controller_menu::slumber(dfw::input& /*input*/)
{

}

bool controller_menu::can_leave_state() const
{
	return true;
}
