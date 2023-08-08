#include "include/resource.h"
#include <error.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>

namespace bonding::resource
{
  Result<Void, error::Err>
  Resource::setup(const std::string hostname) noexcept
  {
    spdlog::debug("Restricting resources for hostname {}", hostname);

    Cgroups::setup(hostname, cgroups);

    return Ok(Void());
  }

  Result<Void, error::Err>
  Cgroups::setup(const std::string hostname,
                 const std::vector<Cgroups::Control> & cgroups) noexcept
  {
    spdlog::debug("Setting cgroups...");

    for (const Cgroups::Control & cgroup : cgroups)
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;

        try
          {
            std::filesystem::create_directories(dir);
          }
        catch (const std::filesystem::filesystem_error & e)
          {
            return Err(error::Err(error::Code::CgroupsError, e.what()));
          }

        for (const Cgroups::Control::Setting & setting : cgroup.settings)
          {
            const std::string path = dir + "/" + setting.name;
            spdlog::debug("Setting {} -> {}", setting.value, path);

            int fd = open(path.c_str(), O_WRONLY);
            if (-1 == fd)
              return Err(error::Err(error::Code::CgroupsError));

            if (-1 == write(fd, setting.value.c_str(), setting.value.length()))
              return Err(error::Err(error::Code::CgroupsError));

            close(fd);
          }
      }

    return Ok(Void());
  }
}
