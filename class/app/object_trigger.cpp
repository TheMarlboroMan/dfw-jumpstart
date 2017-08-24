#include "object_trigger.h"

#include <stdexcept>

using namespace app;

//Construct repeatable.
object_trigger::object_trigger(const app_interfaces::spatiable::t_box& b, ttype tp, int tid)
	:room_object_box{b},
	type{tp},
	unique{false},
	text_id{tid}
{

}

//Construct unique.
object_trigger::object_trigger(const app_interfaces::spatiable::t_box& b, ttype tp, int tid, const std::string& uid)
	:room_object_box{b},
	type{tp},
	unique{true},
	text_id{tid},
	unique_id{uid}
{

}

/*
object_trigger::ttype object_trigger::int_to_type(int v) const
{
	switch(v)
	{
		case ttouch: return touch; break;
		case tactivate: return activate; break;
		default: throw std::runtime_error("Invalid trigger type"); break;
	}

	return touch; //Stupid compiler...
}
*/
