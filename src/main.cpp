/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "spdlog/spdlog.h"
#include "structopt/app.hpp"

#include "include/cli.h"
#include "include/container.h"

using namespace bonding;

int
main(int argc, char ** argv)
{
  spdlog::set_pattern("%H:%M:%S.%f %^>%$ %v");
  
  auto app = structopt::app("bonding");

  try
    {
      auto options = app.parse<cli::Command_Line_Args>(argc, argv);

      if (options.debug.value())
        {
          spdlog::set_level(spdlog::level::debug);
          spdlog::debug("Activate debug mode!");
        }

      container::Container::start(options.to_args()).unwrap();
    }
  catch (structopt::exception & e)
    {
      spdlog::error(e.what());
      spdlog::error(e.help());
    }
  catch (std::bad_optional_access & _)
    {
      spdlog::error(app.help());
    }
  catch (std::exception & e)
    {
      spdlog::error(e.what());
    }
  return 0;
}
