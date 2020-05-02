#pragma once

//tools
#include <tools/options_menu.h>

//libdan2
#include <ldv/group_representation.h>

//std
#include <vector>
#include <map>
#include <functional>

namespace app
{

//!A very simple class to control representation of a tools::options_menu

template <typename T>
class menu_representation
{
	public:

	typedef std::function<void(const T&, std::vector<ldv::representation*>&)> tfunc_register;
	typedef std::function<void(const T&, size_t, const std::vector<ldv::representation*>&, const std::string&, bool)> tfunc_draw;
	typedef std::function<void(const T&, size_t, float, const std::vector<ldv::representation*>&, const std::string&, bool)> tfunc_step;
	using translation_func=std::function<std::string (const std::string&)>;

	//!Creates the object with all assorted functions and data.
	menu_representation(
			tools::options_menu<T>& m, 
			tfunc_register frn,
			tfunc_register frv,
			tfunc_draw fdn,
			tfunc_draw fdv,
			tfunc_step fsn,
			tfunc_step fsv,
			translation_func tft,
			translation_func tfv
		):menu(m),
		f_register_name(frn), f_register_value(frv),
		f_draw_name(fdn), f_draw_value(fdv),
		f_step_name(fsn), f_step_value(fsv),
		current_key(m.get_keys().front()),
		total_options(menu.size()),
		group({0,0}),
		title_translator{tft},
		value_translator{tfv}
	{
		create_data();
		regenerate_representations();
	}

	void				step(float delta)
	{
		size_t i=0;
		for(const auto& k : menu.get_keys()) {

			f_step_name(k, i, delta, name_representations[k], title_translator(k), k==current_key);
			f_step_value(k, i, delta, value_representations[k], value_translator(k), k==current_key);
			++i;
		}
	}

	void				previous() {
		
		current_key=menu.adjacent_key(current_key, tools::options_menu<T>::browse_dir::previous);
	}

	void				next() {

		current_key=menu.adjacent_key(current_key, tools::options_menu<T>::browse_dir::next);
	}

	void				select_option(const T& _key) {

		current_key=_key;
		regenerate_representations();
	}

	//!Returns the key for the current item.
	const T&			get_current_key() const {

		return current_key;
	}

	//!Forwards to "browse" in the current option of the menu.
	void				browse_current(int d) {

		using m=tools::options_menu<T>;

		auto dir=d > 0
			? m::browse_dir::next
			: m::browse_dir::previous;

		menu.browse(current_key, dir);
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
		for(const auto& k : menu.get_keys()) {

			f_draw_name(k, i, name_representations[k], title_translator(k), k==current_key);
			f_draw_value(k, i, value_representations[k], value_translator(k), k==current_key);
			++i;
		}
	}

	void				create_data()
	{
		for(const auto& k : menu.get_keys()) {
		
			name_representations[k]=std::vector<ldv::representation*>{};
			value_representations[k]=std::vector<ldv::representation*>{};
			f_register_name(k, name_representations[k]);
			f_register_value(k, value_representations[k]);
			for(auto &r : name_representations[k]) group.insert(r);
			for(auto &r : value_representations[k]) group.insert(r);
		}
	}

	//references
	tools::options_menu<T>&				menu;

	//functions
	tfunc_register					f_register_name,
							f_register_value;
	tfunc_draw					f_draw_name,
							f_draw_value;
	tfunc_step					f_step_name,
							f_step_value;

	//properties

	T							current_key;
	size_t						total_options;
	ldv::group_representation			group;
	std::map<T, std::vector<ldv::representation*>>	name_representations,
							value_representations; //!< Each key contains a vector of the representations for its key. They are actually owned and managed by the group. So don't clear it.
	translation_func		title_translator,
							value_translator;
};

}
