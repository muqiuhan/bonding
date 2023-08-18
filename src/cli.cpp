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
  function(const Command_Line_Args args) noexcept
  {
    if (args.init.has_value())
      init(args).unwrap();
    else if (args.run.has_value())
      run(args).unwrap();

    return ERR_MSG(error::Code::Cli, "Unknown command line arguments");
  }

  Result<Void, error::Err>
  init(const Command_Line_Args args) noexcept
  {
    return ERR_MSG(error::Code::Cli, "Unimplemented");
  }

  Result<Void, error::Err>
  run(const Command_Line_Args args) noexcept
  {
    return ERR_MSG(error::Code::Cli, "Unimplemented");
  }
}
