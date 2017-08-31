#include "audio_dispatcher.h"

using namespace app;

audio_dispatcher::audio_dispatcher(
	dfw::audio& pa,
	const lda::resource_manager& pam,
	const ldv::rect& pr,
	const tpoint& pp)
	:a(pa), am(pam), view_rect(pr), player_pos(pp)
{

} 

lda::audio_channel audio_dispatcher::request_audio_channel()
{
	return a().get_free_channel();
}

lda::sound& audio_dispatcher::request_sound_resource(size_t i) const
{
	return am.get_sound(i);
}
