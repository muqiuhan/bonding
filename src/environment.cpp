#include "include/environment.h"
#include <dirent.h>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <utility>

namespace bonding::environment
{
  std::expected<std::pair<int, int>, error::Err>
    Kernel::parse_version(const utsname & host)
  {
    int major = 0, minor = 0;

    if (2 != sscanf(host.release, "%u.%u", &major, &minor))
      {
        return std::unexpected(ERR_MSG(
          error::Code::Environment,
          "weird release format: " + std::string(host.release)));
      }

    return std::make_pair(major, minor);
  }

  std::expected<bool, error::Err>
    CgroupsV1::checking_if_controller_supported(const std::string & controller) noexcept
  {
    for (const auto & support_controller :
         std::filesystem::recursive_directory_iterator(PATH))
      {
        if (support_controller.path().filename().c_str() == controller)
          {
            LOG_DEBUG << "Check if Cgroups-v1 " << controller
                      << " controller is supported...✓";

            return true;
          }
      }

    LOG_ERROR << "Check if Cgroups-v1 " << controller << " controller is supported...✗";
    return false;
  }
} // namespace bonding::environment