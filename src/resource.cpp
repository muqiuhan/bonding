/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/resource.h"
#include "include/environment.h"
#include "include/unix.h"
#include "spdlog/spdlog.h"
#include <fcntl.h>
#include <filesystem>
#include <sys/resource.h>
#include <unistd.h>

namespace bonding::resource
{
  Result<Void, error::Err>
  Resource::setup(const std::string & hostname) noexcept
  {
    spdlog::info("Restricting resources for hostname {}", hostname);
    CgroupsV1::setup(hostname).unwrap();
    Rlimit::setup().unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Rlimit::setup() noexcept
  {
    const rlimit rlim = rlimit{ .rlim_cur = NOFILE, .rlim_max = NOFILE };
    if (-1 == setrlimit(RLIMIT_NOFILE, &rlim))
      return ERR(error::Code::Cgroups);

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::setup(const std::string hostname) noexcept
  {
    for (const Control & cgroup : CONFIG)
      {
        if (environment::CgroupsV1::check_support_controller(cgroup.control).unwrap())
          {
            const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
            unix::Filesystem::mkdir(dir).unwrap();

            for (const Control::Setting & setting : cgroup.settings)
              {
                const std::string path = dir + "/" + setting.name;

                int fd = open(path.c_str(), O_WRONLY);
                if (-1 == fd)
                  {
                    close(fd);
                    return ERR_MSG(error::Code::Cgroups,
                                   "Cannot set controller " + setting.name);
                  }

                if (-1 == write(fd, setting.value.c_str(), setting.value.length()))
                  return ERR_MSG(error::Code::Cgroups,
                                 "Cannot set controller " + setting.name);

                spdlog::debug("Setting controller {} by value {}...✓",
                              setting.name,
                              setting.value);
                close(fd);
              }
          }
        else
          return ERR_MSG(error::Code::Cgroups,
                         "Controller " + cgroup.control + " is not support");
      }

    spdlog::info("Setting cgroups by cgroups-v1...✓");
    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::clean(const std::string & hostname) noexcept
  {
    for (const Control & cgroup : CONFIG)
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
        const std::string task = "/sys/fs/cgroup/" + cgroup.control + "/tasks";

        int taskfd = open(task.c_str(), O_WRONLY);
        if (-1 == taskfd)
          return ERR_MSG(error::Code::Cgroups,
                         "Cannot clean cgroups controller " + cgroup.control);

        if (-1 == write(taskfd, "0", 2))
          {
            close(taskfd);
            return ERR_MSG(error::Code::Cgroups,
                           "Cannot clean cgroups controller " + cgroup.control);
          }

        if (-1 == close(taskfd))
          return ERR_MSG(error::Code::Cgroups,
                         "Cannot clean cgroups controller " + cgroup.control);

        if (-1 == rmdir(dir.c_str()))
          return ERR_MSG(error::Code::Cgroups,
                         "Cannot clean cgroups controller " + cgroup.control);
      }

    spdlog::info("Cleaning cgroups-v1 settings...✓");
    return Ok(Void());
  }

  Result<Void, error::Err>
  Resource::clean(const std::string & hostname) noexcept
  {
    CgroupsV1::clean(hostname).unwrap();

    return Ok(Void());
  }
}
