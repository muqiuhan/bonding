/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_HOSTNAME_H
#define BONDING_HOSTNAME_H

#include "error.h"
#include "result.hpp"
#include <ctime>

namespace bonding::hostname
{
  /** A hostname is what identifies machine compared
   ** to every other living on the same network.
   ** It is used by many different networking softwares. */
  class Hostname
  {
  public:
    static Result<Void, error::Err> setup(const std::string & custom) noexcept;
  };
} // namespace bonding::hostname

#endif /* BONDING_HOSTNAME_H */
