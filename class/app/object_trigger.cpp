#include "object_trigger.h"

#include <stdexcept>

using namespace app;

object_trigger::object_trigger(const tbox& b, ttype tp, int tid)
	:room_object_box{b},
	type{tp},
	action_id{tid}
{

}


object_trigger::object_trigger(const object_trigger& o)
	:room_object_box(o),
	type{o.type},
	action_id{o.action_id}
{

}
