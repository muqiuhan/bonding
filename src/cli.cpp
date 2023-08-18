/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "include/configfile.h"
#include "include/hostname.h"
#include <error.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace bonding::cli
{
  Result<Void, error::Err>
  Command_Line_Args::make(const int argc, char * argv[]) noexcept
  {
    const auto parser = init_parser(argc, argv).unwrap();

    if (parser.get<bool>("init"))
      spdlog::info("init");
    else if (parser.get<bool>("run"))
      spdlog::info("run");

    return Ok(Void());
  }

  Result<parser, error::Err>
  Command_Line_Args::init_parser(const int argc, char * argv[]) noexcept
  {
    parser parser(argc, argv);

    parser.add("init",
               "Initialize the current directory as the container directory",
               "init",
               false,
               true);

    parser.add("run",
               "Run with the current directory as the container directory",
               "run",
               false,
               true);

    if (!parser.parse())
      {
        return ERR_MSG(error::Code::Cli, "Cannot parse the command line argument");
        parser.help();
      }

    return Ok(parser);
  }
}
