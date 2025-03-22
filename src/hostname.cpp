/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/hostname.h"
#include <unistd.h>

namespace bonding::hostname
{
  std::expected<void, error::Err> Hostname::setup(const std::string & hostname) noexcept
  {
    if (-1 == sethostname(hostname.c_str(), hostname.size()))
      return std::unexpected(ERR_MSG(
        error::Code::Hostname, "Cannot set hostname " + hostname + " for container"));

    LOG_INFO << "Container hostname is now " << hostname;

    return {};
  }
} // namespace bonding::hostname
