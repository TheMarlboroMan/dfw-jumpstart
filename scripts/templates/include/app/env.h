#pragma once
#include <appenv/env.h>
#include <string>
#include <stdlib.h>

namespace app {

class env:
	public appenv::env {

	public:

	                        env(lm::logger& _logger):
		appenv::env(std::string{".project-placeholder"}, &_logger) 
	{}
};

}
