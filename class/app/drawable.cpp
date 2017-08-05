#include "drawable.h"

using namespace app_interfaces;

bool app_interfaces::drawable_order(const drawable* a, const drawable* b)
{
	if(a->get_ordering_y() < b->get_ordering_y()) return true;
	else if (a->get_ordering_y() > b->get_ordering_y()) return false;
	else return a->get_ordering_x() < b->get_ordering_x();
}
