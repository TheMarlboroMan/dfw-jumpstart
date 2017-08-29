#ifndef AURABLE_H
#define AURABLE_H

#include <audio/audio_channel/audio_channel.h>

namespace app_interfaces
{

//To be implemented and injected into audio players which need a channel.
class channel_dispatcher_interface
{
	public:

	virtual lda::audio_channel	request_audio_channel()=0;
	virtual lda::sound&		request_sound_resource(size_t)const=0;
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

#endif
