/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/hostname.h"
#include "spdlog/spdlog.h"
#include <unistd.h>

namespace bonding::hostname
{
  Result<Void, error::Err>
  Hostname::setup(const std::string & hostname) noexcept
  {
    if (-1 == sethostname(hostname.c_str(), hostname.size()))
      {
        spdlog::error("Cannot set hostname {} for container", hostname);
        return ERR(error::Code::Hostname);
      }

    spdlog::info("Container hostname is now {}", hostname);
    return Ok(Void());
  }
}
