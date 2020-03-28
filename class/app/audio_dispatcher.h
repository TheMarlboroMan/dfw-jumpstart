#pragma once

//local
#include "aurable.h"

//dfw
#include <dfw/audio.h>


//libdan2
#include <lda/audio_channel.h>
#include <lda/sound.h>
#include <lda/resource_manager.h>
#include <ldv/rect.h>
#include <ldt/point_2d.h>

namespace app
{

//Applying a bit of dependency injection: classes that make sound must be
//injected with this one. The interface "aurable" provides a framework for
//this to take place.

struct audio_dispatcher:
		public app_interfaces::channel_dispatcher_interface
{
					audio_dispatcher(
						dfw::audio&,
						const lda::resource_manager&,
						const ldv::rect&,
						const ldt::point_2d<tpos>&);

	virtual lda::audio_channel	request_audio_channel();
	virtual lda::sound&		request_sound_resource(size_t) const;
	virtual const ldv::rect&	request_view_rect() const {return view_rect;}
	virtual const tpoint&		request_player_pos() const {return player_pos;}

	private:

	dfw::audio&			a;
	const lda::resource_manager&	am;
	const ldv::rect&		view_rect;
	const tpoint&			player_pos;
};

}
