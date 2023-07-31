#include "include/mount.h"
#include <sys/mount.h>

namespace bonding::mounts
{
  Result<Unit, error::Err>
  Mount::set() noexcept
  {
    spdlog::info("Setting mount points...");
    __mount_directory("/", MS_PRIVATE);
    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::clean() noexcept
  {
    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::__mount_directory(const std::string & mount_point, unsigned long flags) noexcept
  {
    if (-1 == mount(NULL, mount_point.c_str(), NULL, flags, NULL))
      {
        spdlog::error("Cannot remount {}", mount_point);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Unit());
  }
}