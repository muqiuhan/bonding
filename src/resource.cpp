#include "include/resource.h"
#include "ponci/ponci.hpp"
#include "ponri/ponri.hpp"

namespace bonding::resource
{
  Result<Void, error::Err>
  Resource::setup(const std::string hostname) noexcept
  {
    spdlog::debug("Restricting resources for hostname {}", hostname);
    cgroup_create(hostname);

    return Ok(Void());
  }
}
