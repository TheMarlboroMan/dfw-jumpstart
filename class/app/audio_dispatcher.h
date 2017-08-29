#ifndef AUDIO_DISPATCHER_H
#define AUDIO_DISPATCHER_H

//libdan2
#include <audio/audio_channel/audio_channel.h>

//dfw
#include <class/audio.h>

//local
#include "aurable.h"

namespace app
{

struct audio_dispatcher:
		public app_interfaces::channel_dispatcher_interface
{
					audio_dispatcher(
						dfw::audio& pa,
						const lda::resource_manager& pam);

	virtual lda::audio_channel	request_audio_channel();
	virtual lda::sound&		request_sound_resource(size_t) const;

	dfw::audio&			a;
	const lda::resource_manager&	am;
};

}
#endif
