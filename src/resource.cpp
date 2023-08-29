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
  CgroupsV1::write_settings(const std::string & dir,
                            const CgroupsV1::Control::Setting & setting) noexcept
  {
    const int fd =
      unix::Filesystem::Open(dir + "/" + setting.name, O_WRONLY)
        .or_else([&](const auto & e) {
          return ERR_MSG(error::Code::Cgroups, "Cannot open controller " + setting.name);
        })
        .unwrap();

    unix::Filesystem::Write(fd, setting.value)
      .or_else([&](const auto & e) {
        return ERR_MSG(error::Code::Cgroups,
                       "Cannot write value to controller " + setting.name);
      })
      .unwrap();

    unix::Filesystem::Close(fd)
      .or_else([&](const auto & e) {
        return ERR_MSG(error::Code::Cgroups, "Cannot close controller " + setting.name);
      })
      .unwrap();

    if (setting.name != "task")
      spdlog::debug("Setting controller {} by value {}...✓", setting.name, setting.value);

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::write_contorl(const std::string hostname,
                           const CgroupsV1::Control & cgroup) noexcept
  {
    if (environment::CgroupsV1::check_support_controller(cgroup.control).unwrap())
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
        unix::Filesystem::Mkdir(dir).unwrap();

        for (const Control::Setting & setting : cgroup.settings)
          write_settings(dir, setting).unwrap();

        write_settings(dir, TASK).unwrap();

        return Ok(Void());
      }
    else
      return ERR_MSG(error::Code::Cgroups,
                     "Controller " + cgroup.control + " is not support");
  }

  Result<Void, error::Err>
  CgroupsV1::setup(const std::string hostname) noexcept
  {
    for (const auto & control : CONFIG)
      write_contorl(hostname, control);

    spdlog::info("Setting cgroups by cgroups-v1...✓");
    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::clean_control_task(const CgroupsV1::Control & cgroup) noexcept
  {
    const std::string task = "/sys/fs/cgroup/" + cgroup.control + "/tasks";

    const int taskfd =
      unix::Filesystem::Open(task.c_str(), O_WRONLY)
        .or_else([&](const auto & e) {
          return ERR_MSG(error::Code::Cgroups,
                         "Cannot open the cgroups tasks controller " + cgroup.control);
        })
        .unwrap();

    unix::Filesystem::Write(taskfd, "0")
      .or_else([&](const auto & e) {
        unix::Filesystem::Close(taskfd)
          .or_else([&](const auto & e) {
            return ERR_MSG(error::Code::Cgroups,
                           "Cannot close the cgroups tasks controller " + cgroup.control);
          })
          .unwrap();

        return ERR_MSG(error::Code::Cgroups,
                       "Cannot write the cgroups tasks controller " + cgroup.control);
      })
      .unwrap();

    unix::Filesystem::Close(taskfd)
      .or_else([&](const auto & e) {
        return ERR_MSG(error::Code::Cgroups,
                       "Cannot close the cgroups tasks controller " + cgroup.control);
      })
      .unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::clean(const std::string & hostname) noexcept
  {
    for (const Control & cgroup : CONFIG)
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;

        clean_control_task(cgroup).unwrap();

        unix::Filesystem::Rmdir(dir)
          .or_else([&](const auto & _) {
            return ERR_MSG(error::Code::Cgroups,
                           "Cannot clean cgroups controller " + cgroup.control);
          })
          .unwrap();
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
