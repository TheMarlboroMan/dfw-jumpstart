#include "shared_resources.h"

#include <source/string_utils.h>
#include <class/text_reader.h> //Explode lines from file.

using namespace app;

shared_resources::shared_resources(ldv::resource_manager& rm, ldt::log& l, tools::chrono& cc, tools::arg_manager& am)
	:v_manager(rm), log(l), controller_chrono(cc), arg_manager(am)
#ifdef WDEBUG_CODE
	,debug_config("data/config/debug_config.dnot")
#endif
{
	register_fonts();
	register_tilesets();
	register_animations();
}

void shared_resources::register_fonts()
{	
	log<<"registering fonts..."<<std::endl;

	using namespace tools;

	auto v=explode_lines_from_file("data/resources/fonts.txt");
	for(const auto& l : v)
	{
		auto p=explode(l, '\t');
		if(p.size()!=3)
		{
			throw std::runtime_error("unable to parse font file: 3 parameters expected");
		}
		else
		{
			ttf_manager.insert(p[0], std::atoi( p[1].c_str() ), p[2] );
		}
	}	
}

void shared_resources::register_tilesets()
{
	log<<"registering tilesets..."<<std::endl;

	try
	{
		tilesheets.push_back({"data/app_data/scifitiles-sheet-alt.dat"});
		tilesheets.push_back({"data/app_data/shadows-sheet.dat"});
		tilesheets.push_back({"data/app_data/character-sheet.dat"});
		tilesheets.push_back({"data/app_data/decoration-sheet.dat"});
	}
	catch(std::exception& e)
	{
		log<<"unable to load all tilesets:"<<e.what()<<std::endl;
		throw e;
	}
}

void shared_resources::register_animations()
{
	log<<"registering animations..."<<std::endl;

	try
	{
		//This 2 is "character-sheet", hopefully :).
		animations.push_back({tilesheets[2], "data/app_data/character-animations.dat"});
	}
	catch(std::exception& e)
	{
		log<<"unable to load all animations: "<<e.what()<<std::endl;
		throw e;
	}
}