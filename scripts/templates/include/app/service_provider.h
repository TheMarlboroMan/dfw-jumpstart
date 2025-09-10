#pragma once

#include <memory>

//Lots of forward declarations.
namespace appenv {
	class env;
};

namespace lm {
	class logger;
}

namespace dfw{
	class kernel;
}

namespace dfwimpl{
	class config;
}

namespace ldtools{
	class ttf_manager;
}

namespace app {

//Other forward declarations for app namespace may go here.

class service_provider {

	public:

	service_provider(const appenv::env&, dfwimpl::config&, lm::logger&, dfw::kernel&);
	~service_provider();

	const appenv::env&      get_env() const;
	dfwimpl::config&        get_config();
	lm::logger&             get_logger();
	//ldtools::ttf_manager&   get_ttf_manager();
	//dfw::audio&             get_audio() {return dfwaudio;}


	private:

	const appenv::env&      env;
	dfwimpl::config&        config;
	lm::logger&             logger;
	//dfw::audio&             dfwaudio;
	//std::unique_ptr<ldtools::ttf_manager>	ttf_manager{nullptr};
};
}
