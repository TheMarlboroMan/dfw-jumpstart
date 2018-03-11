#include "step.h"

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_step::controller_step(shared_resources& sr)
	:s_resources(sr)
{

}

void controller_step::loop(dfw::input& input, float delta, int /*step*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	//TODO: Add delta to timer.

	//TODO: shit moves at a fixed speed. Stops once it reaches something.
	
	//TODO: Timer stops once shit is reached.
}

void controller_step::draw(ldv::screen& screen, int fps)
{
	screen.clear(ldv::rgba8(0, 0, 0, 255));

	ldv::box_representation square{ldv::polygon_representation::type::fill, {0,0,20,20}, ldv::rgba8(0, 0, 0, 128)};
	square.set_blend(ldv::representation::blends::alpha);

	//Three fucking FPS XD!.
	for(int x=0; x<(int)screen.get_w(); x+=5) {
		for(int y=0; y<(int)screen.get_h(); y+=5) {
			square.go_to({x, y});
			square.set_color(ldv::rgb8(color.r%255, color.g%2550, color.b%255));
			square.draw(screen);
			color.r+=2; color.g+=3; color.b+=4;
		}
	}

	//TODO: Add something moving along at a fixed speed.

	//TODO: Add a line to signify the limit we must reach.

	//TODO: Draw the timer.

	std::string fdata="fps"+compat::to_string(fps);
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(0, 0, 0, 255), fdata};

	fps_text.align(screen.get_rect(), {ldv::representation_alignment::h::inner_right, ldv::representation_alignment::v::inner_top, 10, 10});
	fps_text.draw(screen);
}
