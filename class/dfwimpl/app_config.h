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
	public dfw::base_config
{
	////////////////////////////////
	// Public interface.

	public:

	app_config();

	struct user_input
	{
		enum devs{keyboard=0, joystick=1, mouse=2, none=3};
		int type, device, code;
	};

	static user_input fw_to_config(dfw::input::input_description& e)
	{
		int type=user_input::none;

		switch(e.type)
		{
			case dfw::input::input_description::types::keyboard: 	type=user_input::keyboard; break;
			case dfw::input::input_description::types::joystick: 	type=user_input::joystick; break;
			case dfw::input::input_description::types::mouse:	type=user_input::mouse; break;
			case dfw::input::input_description::types::none: 	type=user_input::none; break;
		}

		return user_input{type, e.device, e.code};
	};

	//Returns a user_input object to get type (key, joy, mouse...), device (for multiple joysticks) and internal code.
	//The values of the keys (for the config file) can be found in SDL_scancode.h
	user_input token_from_path_input(const std::string& tipo) const
	{	
		const auto& tok=token_from_path("config:input:"+tipo);
		return user_input{tok[0], tok[1], tok[2]};
	}

	//Fullfillment of the kernel interface.

	virtual std::string generate_file_version() const {return "1";}
	virtual std::string get_key_file_version() const {return "config:meta:v";}
	virtual std::string get_key_screen_double_buffer() const {return "config:video:double_buffer";}
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

}
#endif
