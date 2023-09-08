/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "include/log.hpp"
#include <cstdlib>

using namespace bonding;

int
__main(int argc, char ** argv)
{
  log::set_level(LOG_LEVEL_INFO);
  try
    {
      cli::Command_Line_Args::make(argc, argv).unwrap();
    }
  catch (const std::exception & e)
    {
      LOG_ERROR << e.what();
    }

  return 0;
}

int
main(int argc, char ** argv)
{
  return __main(argc, argv);
}
