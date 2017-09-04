#ifndef MENU_REPRESENTATION_H
#define MENU_REPRESENTATION_H

//std
#include <vector>
#include <map>
#include <functional>

//libdan2
#include <video/representation/representation.h>

//tools
#include <templates/options_menu.h>

namespace app
{

struct menu_representation_config
{
	bool				allow_wrap=false; //!< Allows the menu to wrap from the first to last item and vice versa. 
};

//!A very simple class to control representation of a tools::options_menu

/*
The trick here is that four functions must be supplied to do the work of
creating and using the representations...

A simple example:

	std::map<std::string, int>	translation_map;
	tools::mount_from_dnot(tools::dnot_parse_file("data/app_data/menus.dat")["main"], menu, &translation_map);

	std::vector<tools::options_menu<std::string>::translation_struct > trad;
	for(const auto& p: translation_map) trad.push_back({p.first, menu_localization.get(p.second)});
	menu.translate(trad);

	const auto& ttfm=s_resources.get_ttf_manager();

	//Now we can init the representation...
	menu_rep.set_register_name_function([ttfm](const std::string&, std::vector<ldv::representation*>& v)
	{
		v.push_back(new ldv::ttf_representation{
			ttfm.get("consola-mono", 16), 
			ldv::rgba8(255, 255, 255, 255), "", 1., ldv::ttf_representation::text_align::right});
	});
	menu_rep.set_register_value_function([ttfm](const std::string&, std::vector<ldv::representation*>& v)
	{
		v.push_back(new ldv::ttf_representation{
			ttfm.get("consola-mono", 16), 
			ldv::rgba8(255, 255, 255, 255), "", 1., ldv::ttf_representation::text_align::left});
	});
	menu_rep.set_draw_name_function([](const std::string&, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		r.lock_changes();
		r.set_color(current ? ldv::rgb8(255,255,255) : ldv::rgb8(64,64,64));
		r.set_text(val);
		r.unlock_changes();
		r.go_to({0, (int)index*20});
		r.align(ldv::rect{0, 0, 100, 100}, {
			ldv::representation_alignment::h::inner_right,
			ldv::representation_alignment::v::none,
			0,0 });
	});
	menu_rep.set_draw_value_function([](const std::string&, size_t index, const std::vector<ldv::representation*>& v, const std::string& val, bool current)
	{
		auto& r=*(static_cast<ldv::ttf_representation*>(v[0]));
		r.lock_changes();
		r.set_color(current ? ldv::rgb8(255,255,255) : ldv::rgb8(64,64,64));
		r.set_text(val);
		r.unlock_changes();
		r.go_to({0, (int)index*20});
		r.align(ldv::rect{0, 0, 100, 100}, {
			ldv::representation_alignment::h::outer_right,
			ldv::representation_alignment::v::none,
			20,0});
	});
	menu_rep.init();
	menu_rep.draw(screen);

There is no need to assign the "step" functions if "step" is not going to be called.
In any case, there is no checking that the functions are assigned, so crashes are
expected for bad use.
*/

template <typename T>
class menu_representation
{
	public:

	//!Lazy constructor. Needs further calls to assign the functions and a call to "init".
					menu_representation(tools::options_menu<T>& m)
		:menu(m), current_index(0), total_options(0), group({0,0})
	{

	}

	//!Creates the object with all assorted functions and data.
					menu_representation(
			tools::options_menu<T>& m, 
			menu_representation_config cfg,
			std::function<void(const T&, std::vector<ldv::representation*>&)> frn,
			std::function<void(const T&, std::vector<ldv::representation*>&)> frv,
			std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)> fdn,
			std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)> fdv,
			std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)> fsn,
			std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)> fsv
		):menu(m), 
		f_register_name(frn), f_register_value(frv), 
		f_draw_name(fdn), f_draw_value(fdv),
		f_step_name(fsn), f_step_value(fsv),
		config(cfg), current_index(0), total_options(menu.size()),
		group({0,0})
	{
		create_data();
		regenerate_representations();
	}

	//!Companion of the lazy constructor, for cases in which the menu has not been mounted before this object is created.
	void				init()
	{
		total_options=menu.size();
		create_data();
		regenerate_representations();
	}

	void				step(float delta)
	{
		size_t i=0;
		for(const auto& k : menu.get_keys())
		{
			bool current=index_to_t[current_index]==k;
			f_step_name(k, i, delta, name_representations[k], menu.get_name(k), current);
			f_step_value(k, i, delta, name_representations[k], menu.get_title(k), current);
			++i;
		}
	}

	//!Sets the configuration.
	void				set_config(menu_representation_config cfg) {config=cfg;}

	//!Sets the external function. The vector is to be filled with as many representations as needed for the key.
	void				set_register_name_function(std::function<void(const T&, std::vector<ldv::representation*>&)> frn) {f_register_name=frn;}
	void				set_register_value_function(std::function<void(const T&, std::vector<ldv::representation*>&)> frv) {f_register_value=frv;}
	//!Sets the external function. The vector contains the previously inserted representations. A certain amount of static_cast is expected.
	void 				set_draw_name_function(std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)> fdn) {f_draw_name=fdn;}
	void				set_draw_value_function(std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)> fdv) {f_draw_value=fdv;}
	//!Sets the external function. Similar to draw, but a delta value. Will be used if the step function is called.
	void 				set_step_name_function(std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)> fsn) {f_step_name=fsn;}
	void				set_step_value_function(std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)> fsv) {f_step_value=fsv;}

	//!Advances to the next menu item. Will wrap if allowed.
	void				next()
	{
		if(current_index!=total_options-1) ++current_index;
		else if(config.allow_wrap) current_index=0;
		else return;
		regenerate_representations();
	}

	//!Goes back to the previous menu item. Will wrap if allowed.
	void				previous()
	{
		if(current_index) --current_index;
		else if(config.allow_wrap) current_index=total_options-1;
		else return;
		regenerate_representations();
	}

	void				select_option(T k)
	{
		for(const auto& p : index_to_t) 
		{
			if(p.second==k) 
			{
				current_index=p.first;
				regenerate_representations();
				return;
			}
		}
	}

	//!Returns the key for the current item.
	const T&			get_current_key() const {return index_to_t.at(current_index);}

	//!Returns the current index.
	size_t				get_current_index() const {return current_index;}

	//!Forwards to "browse" in the current option of the menu.
	void				browse_current(int d) 
	{	
		menu.browse(index_to_t[current_index], d);
		regenerate_representations();
	}

	//!Sets the position of the group.
	void				set_position(ldv::point p) {group.go_to(p);}

	//!Draws the menu directly to the screen.
	void				draw(ldv::screen& s) {group.draw(s);}

	//!Regenerates representations in case external changes happened.
	void				refresh() {regenerate_representations();}

	//!Provides a handle for the group representation (to use with view_composer, for example).
	//!It is a very bad idea to try and clear the group, as this class has internal references to its items.
	ldv::group_representation&	get_representation() {return group;}

	private:

	void				regenerate_representations()
	{
		size_t i=0;
		for(const auto& k : menu.get_keys())
		{
			bool current=index_to_t[current_index]==k;
			f_draw_name(k, i, name_representations[k], menu.get_name(k), current);
			f_draw_value(k, i, value_representations[k], menu.get_title(k), current);
			++i;
		}
	}

	void				create_data()
	{
		size_t i=0;
		for(const auto& k : menu.get_keys())
		{
			index_to_t[i++]=k;
			name_representations[k]=std::vector<ldv::representation*>{};
			value_representations[k]=std::vector<ldv::representation*>{};
			f_register_name(k, name_representations[k]);
			f_register_value(k, value_representations[k]);
			for(auto &r : name_representations[k]) group.insert(r);
			for(auto &r : value_representations[k]) group.insert(r);
		}
	}

	//references
	tools::options_menu<T>&		menu;

	//functions
	std::function<void(const T&, std::vector<ldv::representation*>&)> 	f_register_name;
	std::function<void(const T&, std::vector<ldv::representation*>&)> 	f_register_value;
	std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)>	f_draw_name;
	std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)>	f_draw_value;
	std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)>	f_step_name;
	std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)>	f_step_value;

	//properties

	menu_representation_config			config;
	size_t						current_index,
							total_options;
	ldv::group_representation			group;
	std::map<T, std::vector<ldv::representation*>>	name_representations,
							value_representations; //!< Each key contains a vector of the representations for its key. They are actually owned and managed by the group. So don't clear it.
	std::map<size_t, T>				index_to_t; //!< Maps index to T.

};

}

#endif
