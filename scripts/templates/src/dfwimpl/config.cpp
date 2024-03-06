#include "../../include/dfwimpl/config.h"

using namespace dfwimpl;

config::config(): dfw::base_config(get_file_path()) {
}

