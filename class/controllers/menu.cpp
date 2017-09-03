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
	menu_localization(0, {"data/app_data/localization/texts"})
{
	mount_menu();
}

void controller_menu::loop(dfw::input& input, float /*delta*/)
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
		switch(menu_rep.get_current_index())
		{
			case 0: break;
			case 1: break;
			case 2: break;
			case 3: break;
			case 4: break;
		}
	}
}

void controller_menu::draw(ldv::screen& screen, int /*fps*/)
{
	menu_rep.get_representation().align(screen.get_rect(), {
			ldv::representation_alignment::h::inner_right,
			ldv::representation_alignment::v::inner_top,
			20,20});

	//TODO: Screen color of the layout is not working.
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

	//Now we can init the representation...
	menu_rep.set_register_name_function([ttfm](const std::string& /*k*/, std::vector<ldv::representation*>& v)
	{
		v.push_back(new ldv::ttf_representation{
			ttfm.get("consola-mono", 16), 
			ldv::rgba8(255, 255, 255, 255), "", 1, ldv::ttf_representation::text_align::right});
	});
	menu_rep.set_register_value_function([ttfm](const std::string& /*k*/, std::vector<ldv::representation*>& /*v*/)
	{
//		v.push_back(new ldv::ttf_representation{
//			ttfm.get("consola-mono", 16), 
//			ldv::rgba8(255, 255, 255, 255), "", 1, ldv::ttf_representation::text_align::left});
	});
	menu_rep.set_draw_name_function([](const std::string& /*k*/, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		r.lock_changes();
		r.set_color(current ? ldv::rgb8(255,255,255) : ldv::rgb8(64,64,64));
		r.set_text(val);
		r.unlock_changes();
		r.go_to({0, (int)index*20});
		r.align(ldv::rect{0, 0, 0, 0}, {
			ldv::representation_alignment::h::inner_right,
			ldv::representation_alignment::v::none,
			0,0 });
	});
	menu_rep.set_draw_value_function([](const std::string& /*k*/, size_t /*index*/, const std::vector<ldv::representation*>& /*v*/, const std::string& /*val*/, bool /*current*/)
	{
//		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
//		r.lock_changes();
//		r.set_color(current ? ldv::rgb8(255,255,255) : ldv::rgb8(64,64,64));
//		r.set_text(val);
//		r.unlock_changes();
//		r.go_to({0, (int)index*20});
//		r.align(ldv::rect{0, 0, 100, 100}, {
//			ldv::representation_alignment::h::outer_right,
//			ldv::representation_alignment::v::none,
//			20,0});
	});
	menu_rep.init();

	layout.register_as_external("menu_selections", menu_rep.get_representation());
//	layout.map_font("main_text_font", s_resources.get_ttf_manager().get("consola-mono", 16));
	layout.parse("data/app_data/layouts.dat", "main_menu_layout");

}
