#include "audio_tools.h"

using namespace app;

int app::solve_line_for_x(app_interfaces::spatiable::t_point p1, app_interfaces::spatiable::t_point p2, tpos x)
{
	//TODO: This, of course, fails for straight or vertical lines.
	double m=(p2.y-p1.y) / (p2.x-p1.x); 	//m=(y2-y1) / (x2-x1)
	double b=p1.y-(m*p1.x); 		//y=mx+b, thus b=y-mx
	return (m*x) + b; 			//y=mx+b.
}


tools::ranged_value<int> app::calculate_volume(int volume, double max_radius, double min_radius, app_interfaces::spatiable::t_point pt1, app_interfaces::spatiable::t_point pt2)
{
	return {0, volume, solve_line_for_x(
		{max_radius, 0.}, 
		{min_radius, (double)volume}, 
		pt1.distance_to(pt2))};
}

lda::sound_panning app::calculate_panning(const app_interfaces::spatiable::t_box& box, app_interfaces::spatiable::t_point p)
{
	return lda::sound_panning::from_left(
		tools::ranged_value<int>{0, 255, solve_line_for_x(
			{box.origin.x, 255.}, 
			{box.origin.x+box.w, 0.}, 
			p.x)});
}
