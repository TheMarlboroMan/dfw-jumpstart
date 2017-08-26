#ifndef OBJECT_TRIGGER_H
#define OBJECT_TRIGGER_H

//std
#include <string>

//local
#include "room_object_box.h"

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
		o.unique==unique &&
		o.text_id==text_id &&
		o.unique_id==unique_id;
	}

				object_trigger(const object_trigger&);
				object_trigger(const app_interfaces::spatiable::t_box&, ttype, int);
				object_trigger(const app_interfaces::spatiable::t_box&, ttype, int, const std::string&);
	ttype			get_type() const {return type;}
	bool			is_touch() const {return type==ttype::touch;}
	bool			is_activate() const {return type==ttype::activate;}
	bool			is_unique() const {return unique;}
	int			get_text_id() const {return text_id;}
	const std::string&	get_unique_id() const {return unique_id;}
	
	private:

//	ttype			int_to_type(int) const;

	ttype			type;
	bool			unique; //Can be activated only once per session.
	int			text_id;
	std::string		unique_id;

};

}

#endif
