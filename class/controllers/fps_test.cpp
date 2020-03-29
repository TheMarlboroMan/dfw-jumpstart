#include "fps_test.h"

//local
#include "../input.h"

//ld
#include <ldv/ttf_representation.h>
#include <ldv/polygon_representation.h>
#include <ldt/sat_2d.h>

//std
#include <cassert>

using namespace app;

controller_fps_test::controller_fps_test(shared_resources& sr)
	:s_resources(sr), frames(0), frames_accum(0), frames_measured(0),
	seconds(0) {

}

void controller_fps_test::loop(dfw::input& input, const dfw::loop_iteration_data& /*lid*/) {
	if(input().is_exit_signal() || input.is_input_down(input_app::escape)) {
		set_leave(true);
		return;
	}
	
}

void controller_fps_test::draw(ldv::screen& screen, int fps) {
	int csec=s_resources.get_controller_chrono().get_seconds(),
		cmsec=s_resources.get_controller_chrono().get_milliseconds();

	++frames;

	if(seconds!=csec) {
		screen.clear(ldv::rgba8(0, 255, 255, 255));
		seconds=csec;
		frames_measured=frames;
		frames_accum+=frames;
		frames=0;
	}
	else {
		screen.clear(ldv::rgba8(0, 0, 0, 0));
	}

	std::string fdata="sec: "+std::to_string(csec)+
	" msec: "+std::to_string(cmsec)+
	" frames:"+std::to_string(frames_accum)+
	" fps"+std::to_string(fps)+"/"+std::to_string(frames_measured);

	//Draw fps.
	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(0, 255, 255, 255), fdata};
	fps_text.go_to({0,0});
	fps_text.draw(screen);

	auto draw_polygon=[&screen](const ldt::polygon_2d<double>& poly, ldv::rgb_color color) {
		std::vector<ldv::point> pts;
		for(const auto& v : poly.get_vertices())
			pts.push_back({(int)v.x, -(int)v.y+400});

		ldv::polygon_representation rep(pts, color);
		rep.draw(screen);
	};


	//We shall use cartesian space for this.
	ldt::polygon_2d<double> poly1{ {{100, 100}, {200, 200}, {200, 100}}},
				poly2{ {{100, 0}, {100, 100}, {200, 0}}};

	ldt::polygon_2d<double> poly3{poly1}, poly4{poly2};

	poly3.move({150,-50});
	poly4.move({150,0});

	ldt::polygon_2d<double> poly5{poly3}, poly6{poly4};

	auto sat_response=SAT_collision_check_mtv(poly6, poly5);

	poly5.move({150,0});
	poly6.move({150,0});

	//Now adjust...
	poly5.move({sat_response.mtv.x, sat_response.mtv.y});

	draw_polygon(poly1, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly2, ldv::rgb8(0, 255,0));
	draw_polygon(poly3, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly4, ldv::rgb8(0, 255,0));
	draw_polygon(poly5, ldv::rgb8(255, 0 ,0));
	draw_polygon(poly6, ldv::rgb8(0, 255,0));

	ldv::ttf_representation txt{
		s_resources.get_ttf_manager().get("consola-mono", 32), 
		ldv::rgba8(255, 255, 255, 255), "mtv:"+std::to_string(sat_response.mtv.x)+","+std::to_string(sat_response.mtv.y)};
	txt.go_to({0, 50});
	txt.draw(screen);
}

void controller_fps_test::awake(dfw::input& /*input*/) {

}

void controller_fps_test::slumber(dfw::input& /*input*/) {

}

bool controller_fps_test::can_leave_state() const {
	return true;
}
