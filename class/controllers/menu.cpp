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
	menu_rep(menu),
	menu_localization(0, {"data/app_data/localization/texts"}),
	flicker{false, false, 1.f}
{
	mount_menu();
}

void controller_menu::loop(dfw::input& input, float delta)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	if(input.is_input_down(input_app::up)) menu_rep.previous();
	else if(input.is_input_down(input_app::down)) menu_rep.next();
	else if(input.is_input_down(input_app::activate)) 
	{
		//An alternative is to use get_current_key() and use std::strings.
		switch(menu_rep.get_current_index())
		{
			case 0: break;
			case 1: break;
			case 2: break;
			case 3: break;
			case 4: set_leave(true); break;
		}
	}

	//There are a few steps...
	menu_rep.step(delta);
	for(auto& b : menu_decorations) b.step(delta);
	pulse.step(delta);
	flicker.step(delta);
	if(flicker.changed)
	{
		tools::int_generator g(0, 300);
		auto &r=*(layout.get_by_id("code_text"));
		r.set_visible(flicker.visible);
		r.go_to(ldv::point(g(), g()));
		//TODO: It would be cool to play a sound just as it changes.
	}
}

void controller_menu::draw(ldv::screen& screen, int /*fps*/)
{
	layout.draw(screen);
}

void controller_menu::mount_menu()
{
	std::map<std::string, int>	translation_map;
	tools::mount_from_dnot(tools::dnot_parse_file("data/app_data/menus.dat")["main"], menu, &translation_map);

	std::vector<tools::options_menu<std::string>::translation_struct > trad;
	for(const auto& p: translation_map) trad.push_back({p.first, menu_localization.get(p.second)});
	menu.translate(trad);

	const auto& ttfm=s_resources.get_ttf_manager();
	const auto& rpul=pulse; //The alpha of the font pulsates...

	auto reg_txt=[ttfm](const std::string& /*k*/, std::vector<ldv::representation*>& v)
	{
		v.push_back(new ldv::ttf_representation{
			ttfm.get("consola-mono", 16), 
			ldv::rgba8(255, 255, 255, 255), "", 1.});
	};

	auto draw_txt_align_right=[](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		r.lock_changes();
			r.set_color(current ? ldv::rgb8(255, 255, 255) : ldv::rgb8(192, 0, 0));
			r.set_text(val);
		r.unlock_changes();
		r.go_to({0, 20+(int)index*20});
		//Align to the inner right of a box that mimics the screen width.
		r.align({0,0,700,1}, {
				ldv::representation_alignment::h::inner_right,
				ldv::representation_alignment::v::none,
				20,0});
	};

	auto tic_txt=[&rpul](const std::string& /*k*/, size_t /*index*/, float /*delta*/, const std::vector<ldv::representation*>& v, const std::string& /*val*/, bool current)
	{
		v[0]->set_alpha(current ? 255 : rpul.val);
	};

	auto reg_empty=[](const std::string&, std::vector<ldv::representation*>&){};
	auto draw_empty=[](const std::string& , size_t, const std::vector<ldv::representation*>&, const std::string&, bool) {};
	auto tic_empty=[](const std::string&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool) {};

	//Now we can init the representation...
	menu_rep.set_register_name_function(reg_txt);
	menu_rep.set_register_value_function(reg_empty);
	menu_rep.set_draw_name_function(draw_txt_align_right);
	menu_rep.set_draw_value_function(draw_empty);
	menu_rep.set_step_name_function(tic_txt);
	menu_rep.set_step_value_function(tic_empty);
	menu_rep.init();

	layout.register_as_external("menu_selections", menu_rep.get_representation());
	layout.map_font("code_font", s_resources.get_ttf_manager().get("consola-mono", 12));
	layout.parse("data/app_data/layouts.dat", "main_menu_layout");

	menu_decorations.push_back({*(layout.get_by_id("boxr")), 0.f, 0.8f});
	menu_decorations.push_back({*(layout.get_by_id("boxg")), 0.3f, 0.4f});
	menu_decorations.push_back({*(layout.get_by_id("boxb")), 0.6f, 1.2f});
}
