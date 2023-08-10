#include "include/environment.h"

namespace bonding::environment
{
  Result<std::pair<int, int>, error::Err>
  Kernel::parse_version(const utsname & host)
  {
    int major = 0, minor = 0;

    if (2 != sscanf(host.release, "%u.%u", &major, &minor))
      {
        return ERR_MSG(error::Code::CheckError,
                       "weird release format: " + std::string(host.release));
      }

    return Ok(std::make_pair(major, minor));
  }

}