#include "fps_test.h"

//std
#include <cassert>

//tools
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_fps_test::controller_fps_test(shared_resources& sr)
	:s_resources(sr), frames(0), frames_accum(0), frames_measured(0),
	seconds(0)
{

}

void controller_fps_test::preloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_fps_test::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}
	
}

void controller_fps_test::postloop(dfw::input& /*input*/, float /*delta*/, int /*fps*/)
{

}

void controller_fps_test::draw(ldv::screen& screen, int fps)
{
	int csec=s_resources.get_controller_chrono().get_seconds(),
		cmsec=s_resources.get_controller_chrono().get_milliseconds();

	++frames;

	if(seconds!=csec)
	{
		screen.clear(ldv::rgba8(0, 255, 255, 255));
		seconds=csec;
		frames_measured=frames;
		frames_accum+=frames;
		frames=0;
	}
	else
	{
		screen.clear(ldv::rgba8(0, 0, 0, 0));
	}

	std::string fdata="sec: "+compat::to_string(csec)+
	" msec: "+compat::to_string(cmsec)+
	" frames:"+compat::to_string(frames_accum)+
	" fps"+compat::to_string(fps)+"/"+compat::to_string(frames_measured);

	//Draw fps.
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(0, 255, 255, 255), fdata};
	fps_text.go_to({0,0});
	fps_text.draw(screen);

	auto draw_polygon=[&screen](const ldt::polygon_2d<double>& poly, ldv::rgb_color color)
	{
		std::vector<ldv::point> pts;
		for(const auto& v : poly.get_vertexes())
			pts.push_back({v.x, v.y});

		ldv::polygon_representation rep(ldv::polygon_representation::type::fill, pts, color);
		rep.draw(screen);
	};


	//We shall use cartesian space for this.
	ldt::polygon_2d<double> poly1{ 
		{
			{100,100}, {200, 100}, {200,200}, {100,200}
		}, {150,150}},
	poly2{
		{
			{100, 300}, {200, 400}, {100, 400}
		}, {150, 350}};

	ldt::polygon_2d<double> poly3{poly1}, poly4{poly2};

	poly3.move({150,150});
	poly4.move({150,0});

	ldt::polygon_2d<double> poly5{poly3}, poly6{poly4};

	auto sat_response=SAT_collision_check_mtv(poly5, poly6, true);
	vector_2d<double> vec=vector_from_angle_and_magnitude((double)sat_response.angle, (double)sat_response.magnitude);

	poly5.move({150,0});
	poly6.move({150,0});

	//Now adjust...
	poly5.move({vec.x, vec.y});

	draw_polygon(poly1, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly2, ldv::rgb8(0, 255,0));
	draw_polygon(poly3, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly4, ldv::rgb8(0, 255,0));
	draw_polygon(poly5, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly6, ldv::rgb8(0, 255,0));
}

void controller_fps_test::awake(dfw::input& /*input*/)
{

}

void controller_fps_test::slumber(dfw::input& /*input*/)
{

}

bool controller_fps_test::can_leave_state() const
{
	return true;
}