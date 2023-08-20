/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "spdlog/spdlog.h"
#include "include/cli.h"

using namespace bonding;

int
main(int argc, char ** argv)
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
