#include "test_poly.h"

#include <sstream>
#include <iomanip>

#include <class/dnot_parser.h>
#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test_poly::controller_test_poly(shared_resources& sr, dfw::signal_dispatcher& /*sd*/)
	:s_resources(sr),
	//TODO: Adjust the last...
	min_zoom{0.3}, max_zoom{1.0}, max_zoom_velocity{600.},
	player_full_stop_threshold{30.}, player_acceleration{1000.},
	player_base_turn_factor{200.}, player_max_turn_velocity{1100.},
	player_max_thrust{15.}, player_min_thrust{-2.5},
	friction_coeficient{1.5},
	state{tstates::play},
	camera{{0,0,700,500},{0,0}},
	player_color(ldv::rgb8(64,64,128)),
	camera_val{1., 1.}
#ifdef WDEBUG_CODE
	,editor_mode(editor_modes::obstacles),
	editor_vertex_rep{ldv::polygon_representation::type::fill, {0,0,10,10}, ldv::rgb8(0,0,128)},
	editor_line_rep({0,0}, {0,0}, ldv::rgb8(0,0,64))
#endif
{
	camera.set_coordinate_system(ldv::camera::tsystem::cartesian);
	reset();
	load();
#ifdef WDEBUG_CODE
	editor_colors={
		ldv::rgb8(51,0,0),	ldv::rgb8(153,0,0),	ldv::rgb8(204,0,0),
		ldv::rgb8(0,51,0),	ldv::rgb8(0,153,0),	ldv::rgb8(0,204,0),
		ldv::rgb8(0,0,51),	ldv::rgb8(0,0,153),	ldv::rgb8(0,0,204),
		ldv::rgb8(51,51,51),	ldv::rgb8(153,153,153),	ldv::rgb8(204,204,204)
	};
	reload_physics_values();
#endif
}

void controller_test_poly::loop(dfw::input& input, const dfw::loop_iteration_data& lid)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape)) {
		//TODO: Nope, back to the previous state.
		set_leave(true);
		return;
	}

#ifdef WDEBUG_CODE 
	if(editor_active) {
		editor_loop(input);
		return;
	}

	if(input.is_input_down(input_app::console_newline)) editor_active=!editor_active;
	if(input.is_input_down(input_app::console_backspace)) reload_physics_values();
#endif

	switch(state)
	{
		//TODO: There should be a demo state.
		//TODO: I could actually make separate objects for each state.
		case tstates::intro:
			//TODO. Hit any key to start.
		break;
		case tstates::play:
			if(input.is_input_pressed(input_app::down))	player_accelerate(lid.delta, -1);
			else if(input.is_input_pressed(input_app::up)) 	player_accelerate(lid.delta, 1);
			else						player_idle(lid.delta);
		
			if(input.is_input_pressed(input_app::left))	player_turn(lid.delta, 1);
			else if(input.is_input_pressed(input_app::right)) player_turn(lid.delta, -1);

			timer.step(lid.delta);
			player_step(lid.delta);
			do_camera(lid.delta);
		break;
		case tstates::game_over:
			//TODO. hit any key to restart
		break;
	}
}

void controller_test_poly::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 0, 0, 255));

#ifdef WDEBUG_CODE
	if(editor_active) {

		editor_draw(screen);
		return;
	}
#endif

	//Draw world... always drawn...
	for(const auto& o : obstacles) draw_polygon(screen, o.poly, o.color, 255); 

	switch(state) {

		//TODO: Read above: maybe use different objects for different states. Add intro state.
		case tstates::intro:
			//TODO. draw logo and hit any key
		break;
		case tstates::play: 
			draw_polygon(screen, player.poly, player_color, 255);
			draw_hud(screen);
		break;
		case tstates::game_over: 
			//TODO. draw particles and hit any key
		break;
	}

#ifdef WDEBUG_CODE
	editor_draw_line(screen, 
		editor_pt(debug_collision_line_pt1.x, debug_collision_line_pt1.y), 
		editor_pt(debug_collision_line_pt2.x, debug_collision_line_pt2.y), 
		ldv::rgb8(255,0,0));

	editor_draw_line(screen, 
		editor_pt(debug_collision_normal_pt1.x, debug_collision_normal_pt1.y), 
		editor_pt(debug_collision_normal_pt2.x, debug_collision_normal_pt2.y), 
		ldv::rgb8(255,0,0));
#endif
}

void controller_test_poly::draw_hud(ldv::screen& screen)
{
	//Draw timer.
	auto timer_str=[](const stimer& t)
	{
		std::stringstream ss;
		ss<<std::setfill('0')<<std::setw(2)<<t.minutes<<":"<<std::setw(2)<<t.seconds<<":"<<std::setw(3)<<t.ms;
		return ss.str();
	};
	ldv::ttf_representation txt_timer{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(255, 255, 255, 255), "time: "+timer_str(timer)+"\nbest: "+timer_str(best_time)};
	txt_timer.align(screen.get_rect(), {ldv::representation_alignment::h::inner_right, ldv::representation_alignment::v::inner_top, 50, 10});
	txt_timer.draw(screen);

	//Draw health.
	//TODO: I'd rather have small objects I can animate.
	ldv::polygon_representation poly_health{ldv::polygon_representation::type::fill, { {0,0}, {40, 0}, {40,10}, {0,10} }, ldv::rgba8(255, 0, 0, 128)};
	poly_health.set_blend(ldv::representation::blends::alpha);
	for(int i=0; i<player.health; ++i)
	{
		poly_health.align(screen.get_rect(), {ldv::representation_alignment::h::inner_right, ldv::representation_alignment::v::inner_top, 0, 10+(14*i)});
		poly_health.draw(screen);
	}	
}

void controller_test_poly::do_camera(float delta)
{
	typedef ldt::point_2d<double>	pnt;
	auto solve_for_x=[](pnt p1, pnt p2, double x)
	{	
		//This, of course, fails for straight or vertical lines.
		double m=(p2.y-p1.y) / (p2.x-p1.x); 	//m=(y2-y1) / (x2-x1)
		double b=p1.y-(m*p1.x); 		//y=mx+b, thus b=y-mx
		return (m*x) + b; 			//y=mx+b.
	};

	camera_val.next=solve_for_x({0., max_zoom}, {max_zoom_velocity, min_zoom}, player.velocity.magnitude());
	camera_val.current+=(camera_val.current < camera_val.next ? 1. : -1. ) * delta * 0.15;
	camera.set_zoom(tools::ranged_value<double>{min_zoom, max_zoom, camera_val.current});
	camera.center_on({(int)player.poly.get_centroid().x, (int)player.poly.get_centroid().y});
}

void controller_test_poly::reset()
{
	soft_reset();
	best_time.reset();
}

void controller_test_poly::soft_reset()
{
	//Reset player.
	player={
		{{ {-16.,-16.},{-16.,16.},{0.,64.},{16.,16.},{16.,-16.} } },
		{0.,0.}, {0.,0.},
		90., 
		max_health
	};
	//Reset camera.
	camera.set_zoom(1.);
	camera.center_on({(int)player.poly.get_centroid().x, (int)player.poly.get_centroid().y});
	waypoint_val.current=1;
	timer.reset();
}

void controller_test_poly::player_accelerate(float delta, double direction)
{
	double thrust_factor=(direction*player_acceleration)*delta;
	player.thrust=ldt::vector_from_angle(player.bearing)*thrust_factor;
}

void controller_test_poly::player_idle(float /*delta*/)
{
	player.thrust={0., 0.};
}

void controller_test_poly::player_turn(float delta, int dir)
{
	//Turning just becomes impossible at certain speeds.
	double	turn_factor=player_base_turn_factor - (player_base_turn_factor * (player.velocity.magnitude() / player_max_turn_velocity));
	if(turn_factor < 0.) return;
	double factor=(double)dir * turn_factor * delta;

	player.poly.rotate(factor);

	//If there's any collision, we undo it and exit early so as to not change the bearing.
	for(const auto& o :obstacles)
	{
		if(ldt::SAT_collision_check(player.poly, o.poly))
		{
			player.poly.rotate(-factor);
			return;
		}
	}

	player.bearing+=factor;

	//Clamp to 0-360.
	if(player.bearing < 0.) player.bearing+=360.;
	else if(player.bearing > 360.) player.bearing-=360.;
}

void controller_test_poly::player_step(float delta)
{
	//There comes a moment where velocity*friction_coeficient*delta == current_acceleration, at which moment we stop accelerating.
	player.velocity-=(player.velocity*friction_coeficient*delta);

	if(!player.thrust.magnitude())
	{
		if(player.velocity.magnitude() < player_full_stop_threshold) player.velocity={0., 0.};
//		else 
//		{
			//TODO: This is problematic: we keep it, reverse becomes forward once key is lifted. We remove it, forget about steering!!!
			//player.velocity=ldt::vector_from_angle(player.bearing)*player.velocity.magnitude();
//		}
	}
	else player.velocity+=player.thrust;

	//Keep maximum speed constant...
//	if(player.velocity.magnitude() > max_magnitude) player.velocity*=max_magnitude / player.velocity.magnitude();

	//Collision detection and response...
	if(player.velocity.x || player.velocity.y) {

		ldt::point_2d<double> p(player.velocity.x * delta, player.velocity.y * delta);
		player.poly.move(p);

#ifdef WDEBUG_CODE
		const auto& dc=s_resources.get_debug_config();
		double 	wrap_max_x=dc.double_from_path("debug:physics:wrap_max_x"),
			wrap_min_x=dc.double_from_path("debug:physics:wrap_min_x"),
			wrap_max_y=dc.double_from_path("debug:physics:wrap_max_y"),
			wrap_min_y=dc.double_from_path("debug:physics:wrap_min_y");
		auto cntrd=player.poly.get_centroid();

		if(cntrd.x > wrap_max_x) player.poly.center_in({wrap_min_x, cntrd.y});
		else if(cntrd.x < wrap_min_x) player.poly.center_in({wrap_max_x, cntrd.y});

		if(cntrd.y > wrap_max_y) player.poly.center_in({cntrd.x, wrap_min_y});
		else if(cntrd.y < wrap_min_y) player.poly.center_in({cntrd.x, wrap_max_y});
#endif

		auto it=std::find_if(std::begin(waypoints), std::end(waypoints), [this](const waypoint& w) {return w.index==waypoint_val.current;});
		if(it!=std::end(waypoints) && ldt::SAT_collision_check(player.poly, it->poly)) do_waypoint();


		for(const auto& o :obstacles) {

			auto sat_res=ldt::SAT_collision_check_mtv(player.poly, o.poly);

			if(sat_res.collision) {

/*
				if(! (--player.health)) {
					soft_reset();
				}
*/

				//Undo movement...
				player.poly.move({-p.x, -p.y});

				auto d=ldt::get_SAT_edge(sat_res, o.poly);
				auto vector_normal=d.vector.left_normal().normalize();

//auto &log=s_resources.get_log();
//log<<"edge "<<d.point.x<<","<<d.point.y<<" to "<<d.end().x<<","<<d.end().y<<std::endl;
//log<<"left normal: "<<vector_normal.x<<","<<vector_normal.y<<std::endl;

				//Formula for reflection goes like:
				// R =v - 2 * (v dot N) * v, where N is normalized.
				auto new_vector=player.velocity - (2. * vector_normal * ldt::dot_product(vector_normal, player.velocity));
				player.velocity=new_vector;
				player.thrust/=3.;

#ifdef WDEBUG_CODE
				//Draw the edge and normal.
				debug_collision_line_pt1=d.point;
				debug_collision_line_pt2=d.end();
				debug_collision_normal_pt1=ldt::segment_middle_point(d);
				debug_collision_normal_pt2=debug_collision_normal_pt1+ldt::point_2d<double>(vector_normal.x*10., vector_normal.y*10.);
/*
log<<"normal is ["
	<<debug_collision_normal_pt1.x<<","<<debug_collision_normal_pt1.y
	<<"] to ["
	<<debug_collision_normal_pt2.x<<","<<debug_collision_normal_pt2.y
	<<"]"<<std::endl;
*/
#endif
				break;
			}
		}
	}
}

void controller_test_poly::do_waypoint() {

	++waypoint_val.current;
	if(waypoint_val.current > waypoint_val.total) {

		waypoint_val.current=1;
		if(!best_time || timer < best_time) best_time=timer;
		timer.reset();
		player.health=max_health;
	}
}

void controller_test_poly::load()
{
	try
	{
		auto root=tools::dnot_parse("data/app_data/arcade_data.dnot");
		obstacles.clear();
		waypoints.clear();

		int index=0;

		for(const auto& t : root["data"]["obstacles"].get_vector()) {

			ldt::polygon_2d<double> poly;
			for(const auto& v: t["poly"].get_vector()) poly.add_vertex({v[0],v[1]});
			if(!poly.is_clockwise() || poly.is_concave()) {
				throw std::runtime_error("counter-clockwise or concave poly found at index ["+std::to_string(index)+"]");
			}

			poly.set_rotation_center(poly.get_centroid());
			ldv::rgb_color color{t["color"].get_vector()[0], t["color"].get_vector()[1], t["color"].get_vector()[2]};

			obstacles.push_back({poly, color});
			++index;
		}

		index=0;
		for(const auto& t : root["data"]["waypoints"].get_vector()) {

			ldt::polygon_2d<double> poly;
			for(const auto& v: t["poly"].get_vector()) poly.add_vertex({v[0],v[1]});

			if(!poly.is_clockwise() || poly.is_concave()) {
				throw std::runtime_error("counter-clockwise or concave waypoint found at index ["+std::to_string(index)+"]");
			}

			poly.set_rotation_center(poly.get_centroid());
			waypoints.push_back({poly, t["index"]});
			++index;
		}

		waypoint_val.total=waypoints.size();
		waypoint_val.current=1;
	}
	catch(std::exception& e)
	{
		std::cout<<"loading failed: "<<e.what()<<std::endl;
	}
}

void controller_test_poly::draw_polygon(ldv::screen& screen, const ldt::polygon_2d<double>& p, ldv::rgb_color color, int alpha)
{
	std::vector<ldv::point> points;
	for(const auto& pt : p.get_vertices()) points.push_back({(int)pt.x, (int)-pt.y});
	ldv::polygon_representation poly{ldv::polygon_representation::type::fill, points, color};
	poly.set_blend(ldv::representation::blends::alpha);
	poly.set_alpha(alpha);
	poly.draw(screen, camera);
#ifdef WDEBUG_CODE
	editor_draw_vertex(screen, editor_pt(p.get_centroid().x, p.get_centroid().y), ldv::rgb8(128,0,0));
	editor_draw_vertex(screen, editor_pt(p.get_rotation_center().x, p.get_rotation_center().y), ldv::rgb8(0,0,128));
#endif
}

void controller_test_poly::draw_raster(ldv::screen& screen, ldv::raster_representation& r)
{
	auto pos=r.get_position();
	r.go_to(ldv::point(pos.x, -pos.y-r.get_view_position().h));
	r.draw(screen, camera);
	r.go_to(pos);
}

//****************************************************
//Editor trash begins...
//****************************************************

#ifdef WDEBUG_CODE

void controller_test_poly::reload_physics_values()
{
	player_acceleration=s_resources.get_debug_config().double_from_path("debug:physics:player_acceleration");
	player_base_turn_factor=s_resources.get_debug_config().double_from_path("debug:physics:player_base_turn_factor");
	player_max_turn_velocity=s_resources.get_debug_config().double_from_path("debug:physics:player_max_turn_velocity");
	player_full_stop_threshold=s_resources.get_debug_config().double_from_path("debug:physics:player_full_stop_threshold");
	player_max_thrust=s_resources.get_debug_config().double_from_path("debug:physics:player_max_thrust");
	player_min_thrust=s_resources.get_debug_config().double_from_path("debug:physics:player_min_thrust");
	friction_coeficient=s_resources.get_debug_config().double_from_path("debug:physics:friction_coeficient");

	std::cout<<"physics values reloaded (if values didn't change remember to hit r):"<<std::endl;
}

void controller_test_poly::editor_loop(dfw::input& input)
{
	editor_mouse_position=input().get_mouse_position();

	if(input().is_mouse_button_down(SDL_BUTTON_LEFT))
	{
		auto pos=editor_cursor_position();
		if(editor_current_poly.size() > 2 && pos==editor_current_poly[0]) {
			editor_close_poly();
		}
		else {
			editor_current_poly.push_back(pos);
		}
	}
	else if(input().is_mouse_button_down(SDL_BUTTON_RIGHT))
	{
		if(editor_current_poly.size()) {
			editor_current_poly.clear();
		}
		else {

			auto p=editor_cursor_position(false);
			ldt::point_2d<double> point{(double)p.x, (double)p.y};

			switch(editor_mode) {
				case editor_modes::obstacles: editor_delete_item(obstacles, point); break;
				case editor_modes::waypoints: editor_delete_item(waypoints, point); break;
			}
		}
	}

	if(input.is_input_pressed(input_app::activate))
	{
		if(input.is_input_down(input_app::left) && camera.get_zoom() > 0.2) 	camera.set_zoom(camera.get_zoom()-0.1);
		else if(input.is_input_down(input_app::right)) 				camera.set_zoom(camera.get_zoom()+0.1);
		
		switch(editor_mode)
		{
			case editor_modes::obstacles:
				if(input.is_input_down(input_app::up)) 		editor_select_color(-1);
				else if(input.is_input_down(input_app::down)) 	editor_select_color(1);
			break;
			case editor_modes::waypoints:
				if(input.is_input_down(input_app::up)) 		editor_select_waypoint(-1);
				else if(input.is_input_down(input_app::down)) 	editor_select_waypoint(1);
			break;
		}

		if(input.is_input_down(input_app::console_newline)) editor_active=!editor_active;;
	}
	else
	{
		if(input.is_input_down(input_app::down)) 	camera.move_by(0, -editor_grid_size/2);
		else if(input.is_input_down(input_app::up)) 	camera.move_by(0, editor_grid_size/2);
		else if(input.is_input_down(input_app::left)) 	camera.move_by(-editor_grid_size/2, 0);
		else if(input.is_input_down(input_app::right)) 	camera.move_by(editor_grid_size/2, 0);

		if(input.is_input_down(input_app::console_backspace)) editor_save();
		if(input.is_input_down(input_app::console_newline)) editor_change_state();
	}
}

void controller_test_poly::editor_save()
{
	using namespace tools;

	//Building the first level map
	dnot_token::t_map mroot;

	//Building the second level maps.
	dnot_token::t_map second_level;

	auto fill_vertexes=[](ldt::polygon_2d<double> poly, tools::dnot_token& tok)
	{
		for(const auto& p: poly.get_vertices())
			tok.get_vector().push_back(dnot_token(dnot_token::t_vector({dnot_token(p.x), dnot_token(p.y)})));
	};

	second_level["obstacles"]=dnot_token(dnot_token::t_vector());
	for(const auto& o : obstacles)
	{
		dnot_token::t_map data;
		data["color"]=dnot_token(dnot_token::t_vector{dnot_token(o.color.r), dnot_token(o.color.g), dnot_token(o.color.b)});
		data["poly"]=dnot_token(dnot_token::t_vector());
		fill_vertexes(o.poly, data["poly"]);
		second_level["obstacles"].get_vector().push_back(dnot_token{data});
	}

	second_level["waypoints"]=dnot_token(dnot_token::t_vector());
	for(const auto& w : waypoints)
	{
		dnot_token::t_map data;
		data["index"]=dnot_token(w.index);
		data["poly"]=dnot_token(dnot_token::t_vector());
		fill_vertexes(w.poly, data["poly"]);
		second_level["waypoints"].get_vector().push_back(dnot_token{data});
	}

	//Adding it all up.
	mroot["data"]=dnot_token(second_level);
	dnot_token root;
	root.set(mroot);

	//Save to disk.
	std::ofstream file("data/app_data/arcade_data.dnot");
	file<<root.serialize();
	file.close();
	
	std::cout<<"saved"<<std::endl;
}

void controller_test_poly::editor_draw(ldv::screen& screen)
{
	editor_draw_grid(screen);

	//Draw obstacles and waypoints...
	for(const auto& o : obstacles) draw_polygon(screen, o.poly, o.color, 255);
	for(const auto& w : waypoints) 
	{
		draw_polygon(screen, w.poly, ldv::rgb8(255, 200, 0), 128);
		ldv::ttf_representation wi{
				s_resources.get_ttf_manager().get("consola-mono", 32), 
				ldv::rgba8(0, 0, 0, 255), compat::to_string(w.index)};

		//Center the text on the polygon...
		auto vt=w.poly.get_vertices();
		int minx=vt[0].x, maxx=minx, miny=vt[0].y, maxy=miny;
		auto f=[](int v, int& min, int& max) {if(v < min) min=v; else if(v > max) max=v;};
		for(const auto& p : vt) {f(p.x, minx, maxx); f(p.y, miny, maxy);}
		wi.align({minx, miny, unsigned(maxx-minx), unsigned(maxy-miny)}, {ldv::representation_alignment::h::center, ldv::representation_alignment::v::center, 0, 0});
		draw_raster(screen, wi);
	}

	//Draw player origin position...
	draw_polygon(screen, player.poly, player_color, 255);

	//Draw current mouse position and polygon in progress...
	editor_draw_vertex(screen, editor_cursor_position(), ldv::rgb8(0,0,128)); 
	editor_draw_current_poly(screen);

	//Other information...
	switch(editor_mode)
	{
		case editor_modes::obstacles:
		{
			ldv::box_representation color_box{ldv::polygon_representation::type::fill, {20,20,20,20}, editor_colors[editor_color_index]};
			color_box.draw(screen);
		}
		break;
		case editor_modes::waypoints:
		{
			ldv::ttf_representation txt_way{
				s_resources.get_ttf_manager().get("consola-mono", 16), 
				ldv::rgba8(0, 255, 255, 255), "index:"+compat::to_string(editor_waypoint_index)};
			txt_way.go_to({0,50});
			txt_way.draw(screen);
		}
		break;
	}

	//HUD data.
	auto pos=editor_cursor_position();
	std::string cdata=
		"cam: "+compat::to_string(camera.get_x())+","+compat::to_string(camera.get_y())
		+" coords: "+compat::to_string(pos.x)+","+compat::to_string(pos.y)
		+" zoom:"+compat::to_string(camera.get_zoom());

	ldv::ttf_representation fps_text{
		s_resources.get_ttf_manager().get("consola-mono", 16), 
		ldv::rgba8(0, 255, 255, 255), cdata};
	fps_text.go_to({0,0});
	fps_text.draw(screen);
}

void controller_test_poly::editor_draw_grid(ldv::screen& screen)
{
	const auto fb=camera.get_focus_box();
	auto color=ldv::rgb8(64,64,64);

	//What is the first and last lines we have to draw in the x coordinate?
	double 	fbar=ceil(fb.origin.x / editor_grid_size),
	//All these doubles are there for a reason: to prevent int casting that won't give us good results.
		lbar=ceil( ( (double)fb.origin.x+(double)fb.w) / (double)editor_grid_size);

	//Multiply the first line for the grid and get to it.
	for(int x=fbar*editor_grid_size; x < lbar * editor_grid_size; x+=editor_grid_size)
		editor_draw_line(screen, {x,fb.origin.y}, {x,fb.origin.y+(int)fb.h}, color);

	//And now with the y...
	fbar=fb.origin.y / editor_grid_size;
	lbar=ceil( ((double)fb.origin.y+(double)fb.h) / (double)editor_grid_size);
	for(int y=fbar*editor_grid_size; y < lbar * editor_grid_size; y+=editor_grid_size)
		editor_draw_line(screen, {fb.origin.x,y}, {fb.origin.x+(int)fb.w,y}, color);
}

void controller_test_poly::editor_draw_current_poly(ldv::screen& screen)
{
	for(const auto& pt: editor_current_poly) editor_draw_vertex(screen, pt, ldv::rgb8(0,128,0));

	auto color=ldv::rgb8(0,0,64);
	if(editor_current_poly.size() >= 1)
	{
		for(size_t i=1; i<editor_current_poly.size(); ++i)
			editor_draw_line(screen, editor_current_poly[i-1], editor_current_poly[i], color); 
		editor_draw_line(screen, editor_current_poly.back(), editor_cursor_position(), color);
	}
}

void controller_test_poly::editor_draw_vertex(ldv::screen& screen, editor_pt pt, ldv::rgb_color color)
{
	editor_vertex_rep.set_color(color);
	editor_vertex_rep.go_to({pt.x-5, -pt.y-5});
	editor_vertex_rep.draw(screen, camera);
}

void controller_test_poly::editor_close_poly()
{
	auto pt=[](editor_pt p)
	{
		return ldt::point_2d<double>{(double)p.x, (double)p.y};
	};
	
	std::vector<ldt::point_2d<double>> vt;
	for(const auto& p : editor_current_poly) vt.push_back(pt(p));

	auto poly=ldt::polygon_2d<double>{vt, {pt(editor_current_poly[0])}};
	if(poly.is_clockwise() && !poly.is_concave()) 
	{
		switch(editor_mode)
		{
			case editor_modes::obstacles:
				obstacles.push_back({poly, editor_colors[editor_color_index]});
			break;
			case editor_modes::waypoints:
				waypoints.push_back({poly, editor_waypoint_index});
			break;
		}
	}

	editor_current_poly.clear();
}

controller_test_poly::editor_pt controller_test_poly::editor_cursor_position(bool snap)
{
	//Mouse snap vertex in world position... Because of the camera's 
	//cartesian system, 0.0 is in the bottom left when the camera points at 0.0.

	const auto fb=camera.get_focus_box();
	int 	mouse_x=fb.origin.x+(editor_mouse_position.x/camera.get_zoom()),
		//No need to alter fb height: camera zoom does it.
		mouse_y=fb.h-(editor_mouse_position.y/camera.get_zoom())+fb.origin.y;

	if(snap)
	{
		mouse_x=floor(mouse_x / editor_grid_size)*editor_grid_size,
		mouse_y=floor(mouse_y / editor_grid_size)*editor_grid_size;
	}

	return {mouse_x, mouse_y};
}

void controller_test_poly::editor_select_color(int d)
{
	editor_color_index+=d;
	if(editor_color_index < 0) editor_color_index=editor_colors.size()-1;
	else if((size_t)editor_color_index==editor_colors.size()) editor_color_index=0;
}

void controller_test_poly::editor_select_waypoint(int d)
{
	editor_waypoint_index+=d;
	if(editor_waypoint_index < 1) editor_waypoint_index=1;
	else if(editor_waypoint_index > 100) editor_waypoint_index=100;
}

void controller_test_poly::editor_draw_line(ldv::screen& screen, ldt::point_2d<int> pt1, ldt::point_2d<int> pt2, ldv::rgb_color color)
{
	editor_line_rep.set_color(color);
	editor_line_rep.set_points({pt1.x, -pt1.y}, {pt2.x, -pt2.y});
	editor_line_rep.draw(screen, camera);
}

void controller_test_poly::editor_change_state()
{
	switch(editor_mode)
	{
		case editor_modes::obstacles: editor_mode=editor_modes::waypoints; break;
		case editor_modes::waypoints: editor_mode=editor_modes::obstacles; break;
	}
}

#endif
