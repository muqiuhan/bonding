#include "spdlog/spdlog.h"
#include "structopt/app.hpp"

#include "include/cli.h"
#include "include/config.h"
#include "include/container.h"

using namespace bonding;

int
main(int argc, char ** argv)
{
  try
    {
      auto options = structopt::app("bonding").parse<cli::Args>(argc, argv);

      if (options.debug.value()) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Activate debug mode!");
      }

      container::Container::start(options);
    }
  catch (structopt::exception & e)
    {
      spdlog::error(e.what());
      spdlog::error(e.help());
    }
  return 0;
}
