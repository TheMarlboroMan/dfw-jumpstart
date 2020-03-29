#pragma once

#include <string>
#include <stdexcept>

namespace app
{

//These are the actions... How about double dispatching these??.

//Forwards.
struct room_action;
struct room_action_text;
struct room_action_exit;    
struct room_action_console;
struct room_action_arcade;    

struct action_dispatcher //Whatever exists in the controller will extend this.
{
	void			resolve(const room_action&) {throw std::runtime_error("Unknown action in dispatcher");}
	virtual void		resolve(const room_action_exit&)=0;
	virtual void		resolve(const room_action_text&)=0;
	virtual void		resolve(const room_action_console&)=0;
	virtual void		resolve(const room_action_arcade&)=0;
};

struct room_action
{
	enum {texit=1, ttext=2, tconsole=3, tarcade=4};
	virtual int 		get_type() const=0;
	virtual void		dispatch(action_dispatcher&)const=0;
				room_action(int aid, bool r)
		:action_id{aid}, repeat{r}
	{
	}

	int			action_id;
	bool			repeat;
};

struct room_action_exit:
	public room_action
{
	virtual int		get_type() const {return texit;}
	virtual void		dispatch(action_dispatcher& ad) const {ad.resolve(*this);}
				room_action_exit(int aid, bool r, const std::string& rm, int tid)
		:room_action(aid, r), room{rm}, terminus_id{tid} {}

	std::string		room;
	int			terminus_id;
};

struct room_action_text:
	public room_action
{
	virtual int		get_type() const {return ttext;}
	virtual void		dispatch(action_dispatcher& ad) const {ad.resolve(*this);}

				room_action_text(int aid, bool r, int tid)
		:room_action(aid, r), text_id(tid) {}

	int			text_id;
};


struct room_action_console:
	public room_action
{
	virtual int		get_type() const {return tconsole;}
	virtual void		dispatch(action_dispatcher& ad) const {ad.resolve(*this);}

				room_action_console(int aid, bool r)
		:room_action(aid, r) {}
};

struct room_action_arcade:
	public room_action
{
	virtual int		get_type() const {return tarcade;}
	virtual void		dispatch(action_dispatcher& ad) const {ad.resolve(*this);}

				room_action_arcade(int aid, bool r)
		:room_action(aid, r) {}
};

} //End namespace.

