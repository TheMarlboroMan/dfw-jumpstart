#include "app/service_provider.h"

#include <dfw/id_to_path_mapper.h>
#include <ldtools/ttf_manager.h>
#include <appenv/env.h>
#include <lm/log.h>
#include <dfw/kernel.h>

using namespace app;

service_provider::service_provider(
	const appenv::env& _env, 
	dfwimpl::config& _config,
	lm::logger& _logger,
	dfw::kernel& /*_kernel*/
):
	env{_env},
	config{_config},
	logger{_logger}
{ }

service_provider::~service_provider() {

	lm::log(logger).info()<<"unmounting service provider...\n";
}

const appenv::env& service_provider::get_env() const {

	return env;
}

dfwimpl::config& service_provider::get_config() {

	return config;
}

lm::logger& service_provider::get_logger() {

	return logger;
}

/*
ldtools::ttf_manager& service_provider::get_ttf_manager() {

	if(nullptr==ttf_manager.get()) {

		ttf_manager.reset(new ldtools::ttf_manager{});
	}

	return *ttf_manager;
}
*/

