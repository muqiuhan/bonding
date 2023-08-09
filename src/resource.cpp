/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/resource.h"
#include <error.h>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/resource.h>

namespace bonding::resource
{
  Result<Void, error::Err>
  Resource::setup(const std::string hostname) noexcept
  {
    spdlog::info("Restricting resources for hostname {}", hostname);

    spdlog::debug("Restriction resources by cgroup-v1...");
    CgroupsV1::setup(hostname).unwrap();

    spdlog::debug("Restriction resources by rlimit...");
    Rlimit::setup().unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Rlimit::setup() noexcept
  {
    const rlimit rlim = rlimit{ .rlim_cur = NOFILE, .rlim_max = NOFILE };
    if (-1 == setrlimit(RLIMIT_NOFILE, &rlim))
      return ERR(error::Code::CgroupsError);

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::setup(const std::string hostname) noexcept
  {
    spdlog::debug("Setting cgroups...");

    for (const Control & cgroup : CONFIG)
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;

        try
          {
            std::filesystem::create_directories(dir);
          }
        catch (const std::filesystem::filesystem_error & e)
          {
            return ERR_MSG(error::Code::CgroupsError, e.what());
          }

        for (const Control::Setting & setting : cgroup.settings)
          {
            const std::string path = dir + "/" + setting.name;

            int fd = open(path.c_str(), O_WRONLY);
            if (-1 == fd)
              {
                close(fd);
                return ERR(error::Code::CgroupsError);
              }

            if (-1 == write(fd, setting.value.c_str(), setting.value.length()))
              return ERR(error::Code::CgroupsError);

            close(fd);
          }
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::clean(const std::string hostname) noexcept
  {
    spdlog::debug("Cleaning cgroups-v1 settings...");

    for (const Control & cgroup : CONFIG)
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
        const std::string task = "/sys/fs/cgroup/" + cgroup.control + "/tasks";

        int taskfd = open(task.c_str(), O_WRONLY);
        if (-1 == taskfd)
          return ERR(error::Code::CgroupsError);

        if (-1 == write(taskfd, "0", 2))
          {
            close(taskfd);
            return ERR(error::Code::CgroupsError);
          }

        close(taskfd);

        if (-1 == rmdir(dir.c_str()))
          return ERR(error::Code::CgroupsError);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Resource::clean(const std::string hostname) noexcept
  {
    CgroupsV1::clean(hostname).unwrap();

    return Ok(Void());
  }
}
