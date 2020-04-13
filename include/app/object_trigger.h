#pragma once

//local
#include "room_object_box.h"

//std
#include <string>

namespace app
{

class object_trigger:
	public room_object_box
{
	public:

//	enum inttype{ttouch=0, tactivate=1};
	enum class ttype{touch, activate};

	bool			operator==(const object_trigger& o) const
	{
		return room_object_box::operator==(o) && 
		o.type==type &&
		o.action_id==action_id;
	}

				object_trigger(const object_trigger&);
				object_trigger(const tbox&, ttype, int);
	bool			is_touch() const {return type==ttype::touch;}
	bool			is_activate() const {return type==ttype::activate;}
	int			get_action_id() const {return action_id;}
	
	private:

	ttype			type;
	int			action_id;

};

}

