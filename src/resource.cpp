/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/resource.h"
#include "include/config.h"
#include "include/environment.h"
#include "include/log.hpp"
#include "include/unix.h"
#include <fcntl.h>
#include <filesystem>
#include <sys/resource.h>
#include <unistd.h>

namespace bonding::resource
{
  Result<Void, error::Err>
    Resource::setup(const config::Container_Options & config) noexcept
  {
    LOG_INFO << "Restricting resources for hostname " << config.hostname;
    CgroupsV1::setup(config).unwrap();
    Rlimit::setup().unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err> Rlimit::setup() noexcept
  {
    const rlimit rlim = rlimit{.rlim_cur = NOFILE, .rlim_max = NOFILE};
    if (-1 == setrlimit(RLIMIT_NOFILE, &rlim))
      return ERR(error::Code::Cgroups);

    LOG_INFO << "Setting rlimit...✓";
    return Ok(Void());
  }

  Result<Void, error::Err> CgroupsV1::write_settings(
    const std::string & dir, const config::CgroupsV1::Control::Setting & setting) noexcept
  {
    const int fd = unix::Filesystem::Open(dir + "/" + setting.name, O_WRONLY)
                     .or_else([&](const auto & e) {
      return ERR_MSG(error::Code::Cgroups, "Cannot open controller " + setting.name);
    }).unwrap();

    unix::Filesystem::Write(fd, setting.value)
      .or_else([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups, "Cannot write value to controller " + setting.name);
    }).unwrap();

    unix::Filesystem::Close(fd)
      .or_else([&](const auto & e) {
      return ERR_MSG(error::Code::Cgroups, "Cannot close controller " + setting.name);
    }).unwrap();

    if (setting.name != "tasks")
      LOG_DEBUG << "Setting controller " << setting.name << "  by value " << setting.value
                << "...✓";

    return Ok(Void());
  }

  Result<Void, error::Err> CgroupsV1::write_contorl(
    const std::string hostname, const config::CgroupsV1::Control & cgroup) noexcept
  {
    if (environment::CgroupsV1::checking_if_controller_supported(cgroup.control).unwrap())
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
        unix::Filesystem::Mkdir(dir).unwrap();

        for (const auto & setting : cgroup.settings)
          write_settings(dir, setting).unwrap();

        write_settings(dir, TASK).unwrap();

        return Ok(Void());
      }
    else
      LOG_WARNING << "Controller " << cgroup.control << " is not support!!";

    return Ok(Void());
  }

  Result<Void, error::Err>
    CgroupsV1::setup(const config::Container_Options & config) noexcept
  {
    for (const auto & control : config.cgroups_options)
      write_contorl(config.hostname, control);

    LOG_INFO << "Setting cgroups by cgroups-v1...✓";
    return Ok(Void());
  }

  Result<Void, error::Err>
    CgroupsV1::clean_control_task(const config::CgroupsV1::Control & cgroup) noexcept
  {
    const std::string task = "/sys/fs/cgroup/" + cgroup.control + "/tasks";

    const int taskfd = unix::Filesystem::Open(task.c_str(), O_WRONLY)
                         .or_else([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot open the cgroups tasks controller " + cgroup.control);
    }).unwrap();

    unix::Filesystem::Write(taskfd, "0")
      .or_else([&](const auto & e) {
      unix::Filesystem::Close(taskfd)
        .or_else([&](const auto & e) {
        return ERR_MSG(
          error::Code::Cgroups,
          "Cannot close the cgroups tasks controller " + cgroup.control);
      }).unwrap();

      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot write the cgroups tasks controller " + cgroup.control);
    }).unwrap();

    unix::Filesystem::Close(taskfd)
      .or_else([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot close the cgroups tasks controller " + cgroup.control);
    }).unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
    CgroupsV1::clean(const config::Container_Options & config) noexcept
  {
    for (const auto & cgroup : config.cgroups_options)
      {
        const std::string dir =
          "/sys/fs/cgroup/" + cgroup.control + "/" + config.hostname;

        clean_control_task(cgroup).unwrap();

        unix::Filesystem::Rmdir(dir)
          .or_else([&](const auto & _) {
          return ERR_MSG(
            error::Code::Cgroups, "Cannot clean cgroups controller " + cgroup.control);
        }).unwrap();
      }

    LOG_INFO << "Cleaning cgroups-v1 settings...✓";
    return Ok(Void());
  }

  Result<Void, error::Err>
    Resource::clean(const config::Container_Options & config) noexcept
  {
    CgroupsV1::clean(config).unwrap();

    return Ok(Void());
  }
} // namespace bonding::resource
