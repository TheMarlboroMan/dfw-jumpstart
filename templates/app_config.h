#ifndef APP_CONFIGURATION_H
#define APP_CONFIGURATION_H

//std
#include <iostream>

//framework
#include <class/base_config.h>
#include <class/input.h>


namespace app
{

class app_config:
	public dfw::base_config {
	////////////////////////////////
	// Public interface.

	public:

	app_config();

	//Fullfillment of the kernel interface.

	virtual std::string generate_file_version() const {return "1";}
	virtual std::string get_key_file_version() const {return "config:meta:v";}
	virtual std::string get_key_screen_double_buffer() const {return "config:video:double_buffer";}
	virtual std::string get_key_screen_vsync() const {return "config:video:vsync";}
	virtual std::string get_key_sound_volume() const {return "config:audio:sound_volume";}
	virtual std::string get_key_music_volume() const {return "config:audio:music_volume";}
	virtual std::string get_key_audio_ratio() const {return "config:audio:audio_ratio";}
	virtual std::string get_key_audio_buffers() const {return "config:audio:audio_buffers";}
	virtual std::string get_key_audio_out() const {return "config:audio:audio_out";}
	virtual std::string get_key_audio_channels() const {return "config:audio:audio_channels";}

	///////////////////////////////////
	// Properties.

	private:

	std::string get_file_path() const {return "data/config/config.dnot";}
};

dfw::input_description 		input_description_from_config_token(const tools::dnot_token&);
tools::dnot_token		config_token_from_input_description(const dfw::input_description&);
dfw::input_description::types	input_description_type_from_int(int);
int				input_description_int_from_type(dfw::input_description::types);

}
#endif
