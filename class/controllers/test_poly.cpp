#include "test_poly.h"

#include <templates/compatibility_patches.h>

//local
#include "../input.h"

using namespace app;

controller_test_poly::controller_test_poly(shared_resources& sr, dfw::signal_dispatcher& /*sd*/)
	:s_resources(sr),
	camera{{0,0,700,500},{0,0}},
	editor_vertex_rep{ldv::polygon_representation::type::fill, {0,0,10,10}, ldv::rgb8(0,0,128)},
	editor_line_rep({0,0}, {0,0}, ldv::rgb8(0,0,64))
{
	camera.set_coordinate_system(ldv::camera::tsystem::cartesian);
	editor_load();

	editor_colors={
		ldv::rgb8(51,0,0),	ldv::rgb8(153,0,0),	ldv::rgb8(204,0,0),
		ldv::rgb8(0,51,0),	ldv::rgb8(0,153,0),	ldv::rgb8(0,204,0),
		ldv::rgb8(0,0,51),	ldv::rgb8(0,0,153),	ldv::rgb8(0,0,204),
		ldv::rgb8(51,51,51),	ldv::rgb8(153,153,153),	ldv::rgb8(204,204,204)
	};
	editor_color_index=0;
}

void controller_test_poly::loop(dfw::input& input, float /*delta*/)
{
	if(input().is_exit_signal() || input.is_input_down(input_app::escape))
	{
		set_leave(true);
		return;
	}

	editor_loop(input);
}

void controller_test_poly::draw(ldv::screen& screen, int /*fps*/)
{
	screen.clear(ldv::rgba8(0, 0, 0, 255));

	editor_draw(screen);
}

void controller_test_poly::editor_loop(dfw::input& input)
{
	editor_mouse_position=input().get_mouse_position();

	if(input().is_mouse_button_down(SDL_BUTTON_LEFT))
	{
		auto pos=editor_cursor_position();
		if(editor_current_poly.size() > 2 && pos==editor_current_poly[0])
		{
			editor_close_poly();
		}
		else
		{
			editor_current_poly.push_back(pos);
		}
	}
	else if(input().is_mouse_button_down(SDL_BUTTON_RIGHT))
	{
		if(editor_current_poly.size())
		{
			editor_current_poly.clear();
		}
		else
		{
			auto p=editor_cursor_position(false);
			ldt::point_2d<double> point{(double)p.x, (double)p.y};

			auto it=std::remove_if(std::begin(obstacles), std::end(obstacles),
				[point](const obstacle& o) {return ldt::point_in_polygon<double>(o.poly, point);});
			if(it!=std::end(obstacles)) obstacles.erase(it);
		}
	}

	if(input.is_input_down(input_app::console_backspace)) editor_save();

	if(input.is_input_pressed(input_app::activate))
	{
//TODO: Zooming is awful, as it adjusts from the top and breaks as the camera moves.
//TODO: Perhaps that needs to be fixed in the libdan... Also, as zoom is done, the camera position breaks somewhat and objects do not appear..

		if(input.is_input_down(input_app::left) && camera.get_zoom() > 0.2) 	camera.set_zoom(camera.get_zoom()-0.1);
		else if(input.is_input_down(input_app::right)) 				camera.set_zoom(camera.get_zoom()+0.1);
		
		if(input.is_input_down(input_app::up)) 		editor_select_color(-1);
		else if(input.is_input_down(input_app::down)) 	editor_select_color(1);
	}
	else
	{
		if(input.is_input_down(input_app::down)) 	camera.move_by(0, -editor_grid_size/2);
		else if(input.is_input_down(input_app::up)) 	camera.move_by(0, editor_grid_size/2);
		else if(input.is_input_down(input_app::left)) 	camera.move_by(-editor_grid_size/2, 0);
		else if(input.is_input_down(input_app::right)) 	camera.move_by(editor_grid_size/2, 0);
	}

	//TODO: Take into account collisions when rotating!.
	//TODO: Arrow pointing to next waypoint.
	//TODO: Count waypoints so we know when to loop!.
}

void controller_test_poly::editor_save()
{
	using namespace tools;

	//Building the first level map
	dnot_token::t_map mroot;

	//Building the second level maps.
	dnot_token::t_map second_level;
	second_level["obstacles"]=dnot_token(dnot_token::t_vector());
	for(const auto& o : obstacles)
	{
		dnot_token::t_map data;
		data["color"]=dnot_token(dnot_token::t_vector{dnot_token(o.color.r), dnot_token(o.color.g), dnot_token(o.color.b)});
		data["poly"]=dnot_token(dnot_token::t_vector());
		for(const auto& p: o.poly.get_vertexes())
			data["poly"].get_vector().push_back(dnot_token(dnot_token::t_vector({dnot_token(p.x), dnot_token(p.y)})));

		second_level["obstacles"].get_vector().push_back(dnot_token{data});
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

void controller_test_poly::editor_load()
{
	try
	{
		auto root=tools::dnot_parse("data/app_data/arcade_data.dnot");
		obstacles.clear();

		for(const auto& t : root["data"]["obstacles"].get_vector())
		{
			ldv::rgb_color color{t["color"].get_vector()[0], t["color"].get_vector()[1], t["color"].get_vector()[2]};
			ldt::polygon_2d<double> poly;
			for(const auto& v: t["poly"].get_vector()) poly.add_vertex({v[0],v[1]});
			obstacles.push_back({poly, color});
		}
	}
	catch(std::exception& e)
	{
		std::cout<<"loading failed: "<<e.what()<<std::endl;
	}
}

void controller_test_poly::editor_draw(ldv::screen& screen)
{
	auto transform=[&screen](const ldt::polygon_2d<double>& p, ldv::rgb_color color)
	{
		std::vector<ldv::point> points;
		for(const auto& pt : p.get_vertexes()) points.push_back({(int)pt.x, (int)-pt.y});
		return ldv::polygon_representation{ldv::polygon_representation::type::fill, points, color};
	};

	editor_draw_grid(screen);

	for(const auto& o : obstacles) transform(o.poly, o.color).draw(screen, camera);
	editor_draw_vertex(screen, editor_cursor_position()); //Draw current mouse position.
	editor_draw_current_poly(screen);

	ldv::box_representation color_box{ldv::polygon_representation::type::fill, {20,20,20,20}, editor_colors[editor_color_index]};
	color_box.draw(screen);

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
	//TODO: Still a bit fussy on the last part of th grid.

	const auto fb=camera.get_focus_box();
	auto color=ldv::rgb8(64,64,64);

	//What is the first and last lines we have to draw in the x coordinate?
	int 	fbar=ceil(fb.origin.x / editor_grid_size),
		lbar=ceil(fb.origin.x+(int)fb.w) / editor_grid_size;

	//Multiply the first line for the grid and get to it.
	for(int x=fbar*editor_grid_size; x < lbar * editor_grid_size; x+=editor_grid_size)
		editor_draw_line(screen, {x,fb.origin.y}, {x,fb.origin.y+(int)fb.h}, color);

	//And now with the y...
	fbar=fb.origin.y / editor_grid_size;
	lbar=ceil(fb.origin.y+(int)fb.h) / editor_grid_size;

	for(int y=fbar*editor_grid_size; y < lbar * editor_grid_size; y+=editor_grid_size)
		editor_draw_line(screen, {fb.origin.x,y}, {fb.origin.x+(int)fb.w,y}, color);
}

void controller_test_poly::editor_draw_current_poly(ldv::screen& screen)
{
	for(const auto& pt: editor_current_poly) editor_draw_vertex(screen, pt);

	auto color=ldv::rgb8(0,0,64);
	if(editor_current_poly.size() >= 1)
	{
		for(size_t i=1; i<editor_current_poly.size(); ++i)
			editor_draw_line(screen, editor_current_poly[i-1], editor_current_poly[i], color); 
		editor_draw_line(screen, editor_current_poly.back(), editor_cursor_position(), color);
	}
}

void controller_test_poly::editor_draw_vertex(ldv::screen& screen, editor_pt pt)
{
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
	if(poly.is_clockwise() && !poly.is_concave()) obstacles.push_back({poly, editor_colors[editor_color_index]});

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

void controller_test_poly::editor_draw_line(ldv::screen& screen, ldt::point_2d<int> pt1, ldt::point_2d<int> pt2, ldv::rgb_color color)
{
	editor_line_rep.set_color(color);
	editor_line_rep.set_points({pt1.x, -pt1.y}, {pt2.x, -pt2.y});
	editor_line_rep.draw(screen, camera);
}
