#ifndef GAME_DEFS_H
#define GAME_DEFS_H

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
	//TODO: Maybe from vector too...
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
	
	//TODO: Shit from vectors.
};

struct game_defs
{


};

}

#endif
