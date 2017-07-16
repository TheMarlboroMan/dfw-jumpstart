#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

//Framework
#include <class/kernel_config_interface.h>

//Local
#include "app_config.h"

namespace app
{

class kernel_config
	:public dfw::kernel_config_interface
{
	public:

	kernel_config(const app_config&);

	virtual std::vector<std::string> 	get_sound_entries() const;
	virtual std::vector<std::string> 	get_music_entries() const;
	virtual std::vector<std::string> 	get_texture_entries() const;
	virtual std::vector<std::string> 	get_surface_entries() const;

	//Información para ventana.
	virtual dfw::window_info		get_window_info() const;

	//Información del input.
	virtual std::vector<dfw::input_pair> 	get_input_pairs() const;

	private:

	const app_config&			config;
};

}

#endif
