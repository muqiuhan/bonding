/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "spdlog/spdlog.h"
#include <cstdlib>

using namespace bonding;

int
__main(int argc, char ** argv)
{
  spdlog::set_pattern("%H:%M:%S.%f %^>%$ %v");

  try
    {
      cli::Command_Line_Args::make(argc, argv).unwrap();
    }
  catch (const std::exception & e)
    {
      spdlog::error(e.what());
    }

  return 0;
}

int
main(int argc, char ** argv)
{
  return __main(argc, argv);
}
