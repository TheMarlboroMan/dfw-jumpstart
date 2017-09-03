#include "localization.h"

using namespace app;

//The list of files is piped through, so we don't hardcode it.
localization::localization(short unsigned int lan, const t_filename& val)
	:localization_base(lan), 
	file_list(val), 
	not_loaded("localization file not ready"),
	not_found("localization string not found")
{
	init();
}
