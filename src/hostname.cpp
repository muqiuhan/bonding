/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/hostname.h"
#include "include/log.hpp"
#include <unistd.h>

namespace bonding::hostname
{
  Result<Void, error::Err> Hostname::setup(const std::string & hostname) noexcept
  {
    if (-1 == sethostname(hostname.c_str(), hostname.size()))
      return ERR_MSG(
        error::Code::Hostname, "Cannot set hostname " + hostname + " for container");

    LOG_INFO << "Container hostname is now " << hostname;
    return Ok(Void());
  }
} // namespace bonding::hostname
