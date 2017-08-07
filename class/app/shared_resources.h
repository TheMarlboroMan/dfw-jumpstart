#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

//The place where all resources live: animation tables, sprites, fonts, log...
//Many of these things are property of the kernel, but I don't fancy just 
//passing the kernel along, neither updating the framework to create a module
//specially tailored to this thing.

//Considering that this is a mismash of unrelated functionality, this is the
//culminating point of lazyness... But it is also easy and clear, mind you.

//Libdan
#include <tools/log/log.h>
#include <video/resource_manager/resource_manager.h>

//Tools
#include <class/ttf_manager.h>

namespace app
{

class shared_resources
{
	public:

	shared_resources(ldv::resource_manager&, ldt::log&);
	const tools::ttf_manager&	get_ttf_manager() const {return ttf_manager;}
	const ldv::resource_manager&	get_video_resource_manager() const {return v_manager;}
	ldt::log&			get_log() {return log;}

	private:
	
	void						register_fonts();
	void						register_tilesets();

	//References
	ldv::resource_manager&				v_manager;
	ldt::log&					log;

	//Properties
	tools::ttf_manager				ttf_manager;
};

}

#endif
