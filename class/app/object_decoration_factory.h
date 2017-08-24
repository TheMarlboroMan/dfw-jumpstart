#ifndef OBJECT_DECORATION_FACTORY_H
#define OBJECT_DECORATION_FACTORY_H

//Tools.
#include <class/dnot_parser.h>

//Local
#include "object_decoration.h"

namespace app
{
class object_decoration_factory
{
	public:

					object_decoration_factory();
	object_decoration		make_object(const tools::dnot_token&);

	private:

	tools::dnot_token::t_map	data;

};
}

#endif
