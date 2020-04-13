#pragma once

//local
#include "app_defs.h"

//libdansd2l
#include <lda/audio_channel.h>
#include <ldv/rect.h>

namespace app
{

//To be implemented and injected into audio players which need a channel.
class channel_dispatcher_interface
{
	public:

	virtual lda::audio_channel	request_audio_channel()=0;
	virtual lda::sound&		request_sound_resource(size_t)const=0;
	virtual const ldv::rect&	request_view_rect() const=0;
	virtual const app::tpoint&	request_player_pos() const=0;
};

//The aurable needs to get injected with a channel dispatcher, which it will
//use to request audio channels.

class aurable
{
	public:

	void					inject_dispatcher(channel_dispatcher_interface& i)
	{
		dispatcher=&i;
	}

	channel_dispatcher_interface *		dispatcher=nullptr;
};

} //End namespace...

