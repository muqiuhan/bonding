#include "include/cli.h"
#include "structopt/app.hpp"
#include <spdlog/spdlog.h>

namespace bonding::cli
{
  Args
  Command_Line_Args::to_args()
  {
    if (!command.has_value())
      spdlog::error("The `command` parameter must be provided!");

    if (!mount_dir.has_value())
      spdlog::error("The `mount-dir` parameter must be provided!");

    if (!uid.has_value())
      spdlog::error("The `uid` parameter must be provided!");

    return Args{ debug.value(), command.value(), uid.value(), mount_dir.value() };
  }
}