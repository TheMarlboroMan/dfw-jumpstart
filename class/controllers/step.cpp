#include "step.h"

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_step::controller_step(shared_resources& sr)
	:s_resources(sr)
{
	data.pos=0.f;
	data.speed=500.f;
	data.max_limit=500.f;
	data.elapsed=0.f;
}

void controller_step::loop(dfw::input& input, float delta, int /*step*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	if(data.pos < data.max_limit) {
		data.elapsed+=delta;
		data.pos+=data.speed*delta;
	}
}

void controller_step::draw(ldv::screen& screen, int fps)
{
	screen.clear(ldv::rgba8(0, 0, 0, 255));

	ldv::box_representation square{ldv::polygon_representation::type::fill, {0,0,20,20}, ldv::rgba8(0, 0, 0, 128)};
	square.set_blend(ldv::representation::blends::alpha);

	//Three fucking FPS XD!.
	for(int x=0; x<(int)screen.get_w(); x+=15) {
		for(int y=0; y<(int)screen.get_h(); y+=15) {
			square.go_to({x, y});
			square.set_color(ldv::rgb8(color.r%255, color.g%2550, color.b%255));
			square.draw(screen);
			color.r+=2; color.g+=3; color.b+=4;
		}
	}

	square.set_color(ldv::rgb8(255, 0, 0));
	square.set_alpha(255);
	square.go_to({(int)data.pos-20, 200});
	square.draw(screen);

	ldv::line_representation goal({(int)data.max_limit, 0}, {(int)data.max_limit, 400}, ldv::rgb8(255, 0, 0));
	goal.draw(screen);

	std::string fdata="fps:"+compat::to_string(fps)+" elapsed:"+compat::to_string(data.elapsed);
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(255, 255, 255, 255), fdata};

	fps_text.align(screen.get_rect(), {ldv::representation_alignment::h::inner_right, ldv::representation_alignment::v::inner_top, 10, 10});
	fps_text.draw(screen);
}
