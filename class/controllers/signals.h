#pragma once

#include <dfw/signal_broadcasting.h>

namespace app
{

//broadcasting... this class registers itself on construction: the best
//way to never forget about it. It is designed to be a member of the receiver
//class, that then injects a lambda (f) onto it to forward the call to its
//own members.
struct app_receiver:
	public dfw::signal_receiver
{

					app_receiver(dfw::signal_dispatcher& sd)
	{
		sd.register_receiver(*this);
	}

	virtual void			receive_signal(const dfw::broadcast_signal& bs)
	{
		f(bs);
	}
	
	std::function<void(const dfw::broadcast_signal&)>	f;
};

enum signal_types{
	t_signal_text_display,
	t_signal_video_size,
	t_signal_video_vsync,
	t_signal_audio_volume,
	t_signal_music_volume,
	t_signal_save_configuration,
	t_signal_save_controls,
	t_signal_reset_state,
};

struct signal_text_display:public dfw::broadcast_signal
{
					signal_text_display(const std::string& v):text(v) {}
	virtual int 			get_type() const {return t_signal_text_display;}
	std::string			text;
	
};

struct signal_video_size:public dfw::broadcast_signal
{
					signal_video_size(const std::string& v):value(v) {}
	virtual int 			get_type() const {return t_signal_video_size;}
	std::string			value;
};

struct signal_video_vsync:public dfw::broadcast_signal
{
					signal_video_vsync(bool v):value(v) {}
	virtual int 			get_type() const {return t_signal_video_vsync;}
	bool				value;
};

struct signal_audio_volume:public dfw::broadcast_signal
{
					signal_audio_volume(int v):value(v) {}
	virtual int 			get_type() const {return t_signal_audio_volume;}
	int				value;	
};

struct signal_music_volume:public dfw::broadcast_signal
{
					signal_music_volume(int v):value(v) {}
	virtual int 			get_type() const {return t_signal_music_volume;}
	int				value;
};

struct signal_save_configuration:public dfw::broadcast_signal
{
	virtual int 			get_type() const {return t_signal_save_configuration;}
};

struct signal_save_controls:public dfw::broadcast_signal
{
	virtual int 			get_type() const {return t_signal_save_controls;}
};

struct signal_reset_state:public dfw::broadcast_signal
{
	virtual int 			get_type() const {return t_signal_reset_state;}
};

}
