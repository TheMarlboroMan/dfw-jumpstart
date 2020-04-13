#include "../../include/app/drawable.h"

using namespace app;

bool app::drawable_order(const drawable* a, const drawable* b)
{
	if(a->get_ordering_y() < b->get_ordering_y()) return true;
	else if (a->get_ordering_y() > b->get_ordering_y()) return false;
	else return a->get_ordering_x() < b->get_ordering_x();
}
