#pragma once

#include "app_defs.h"

#include <ldt/vector_2d.h>

namespace app
{

struct bearing
{
	enum class tbearing{
		n, ne, e, se, 
		s, sw, w, nw
	}val;

	bearing():
		val(tbearing::n)
	{}

	bearing(int v):
		val(tbearing::n)
	{
		this->operator=(v);
	}

	tbearing operator()() const {return val;}

	bearing& operator=(tbearing v) {val=v; return *this;}
	bearing& operator=(const bearing& v) {val=v.val; return *this;}
	bearing& operator=(const ldt::vector_2d<tpos>& v) 
	{
		tpos def_tpos={}; //This is meant to be a zero value.

		if(!v.y && !v.x) {}
		else if(v.x && v.y)
		{
			if(v.x > def_tpos)
			{
				if(v.y > def_tpos) val=tbearing::se;
				else val=tbearing::ne;
			}
			else
			{
				if(v.y > def_tpos) val=tbearing::sw;
				else val=tbearing::nw;
			}
		}
		else if(v.x && !v.y)
		{
			if(v.x > def_tpos) val=tbearing::e;
			else val=tbearing::w;
		}
		else if(v.y && !v.x)
		{
			if(v.y > def_tpos) val=tbearing::s;
			else val=tbearing::n;
		}
		
		return *this;
	}

	bearing& operator=(int v) 
	{
		switch(v)
		{
			case 0:
			default: 	val=tbearing::n; break;
			case 1:		val=tbearing::ne; break;
			case 2:		val=tbearing::e; break;
			case 3:		val=tbearing::se; break;
			case 4:		val=tbearing::s; break;
			case 5:		val=tbearing::sw; break;
			case 6:		val=tbearing::w; break;
			case 7:		val=tbearing::nw; break;
		}

		return *this;
	}
};

}
