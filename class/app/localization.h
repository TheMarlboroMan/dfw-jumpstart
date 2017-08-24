#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <base/localization_base.h>

namespace app
{

class localization:
	public tools::localization_base
{
	public:

				localization(short unsigned int, const t_filename&);
		
	virtual t_filename 	get_file_list() {return file_list;}
	virtual t_string const& string_not_loaded() const {return not_loaded;}
	virtual t_string const& string_not_found() const {return not_found;}

	private:

	t_filename		file_list;
	t_string		not_loaded;
	t_string		not_found;
};

}

#endif
