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
  std::expected<void, error::Err>
    Resource::setup(const config::Container_Options & config) noexcept
  {
    LOG_INFO << "Restricting resources for hostname " << config.hostname;
    CgroupsV1::setup(config).value();
    Rlimit::setup().value();

    return {};
  }

  std::expected<void, error::Err> Rlimit::setup() noexcept
  {
    const rlimit rlim = rlimit{.rlim_cur = NOFILE, .rlim_max = NOFILE};
    if (-1 == setrlimit(RLIMIT_NOFILE, &rlim))
      return std::unexpected(ERR(error::Code::Cgroups));

    LOG_INFO << "Setting rlimit...✓";
    return {};
  }

  std::expected<void, error::Err> CgroupsV1::write_settings(
    const std::string & dir, const config::CgroupsV1::Control::Setting & setting) noexcept
  {
    const int fd = unix::Filesystem::Open(dir + "/" + setting.name, O_WRONLY)
                     .transform_error([&](const auto & e) {
      return ERR_MSG(error::Code::Cgroups, "Cannot open controller " + setting.name);
    }).value();

    unix::Filesystem::Write(fd, setting.value)
      .transform_error([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups, "Cannot write value to controller " + setting.name);
    }).value();

    unix::Filesystem::Close(fd)
      .transform_error([&](const auto & e) {
      return ERR_MSG(error::Code::Cgroups, "Cannot close controller " + setting.name);
    }).value();

    if (setting.name != "tasks")
      LOG_DEBUG << "Setting controller " << setting.name << "  by value " << setting.value
                << "...✓";

    return {};
  }

  std::expected<void, error::Err> CgroupsV1::write_contorl(
    const std::string hostname, const config::CgroupsV1::Control & cgroup) noexcept
  {
    if (environment::CgroupsV1::checking_if_controller_supported(cgroup.control))
      {
        const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
        unix::Filesystem::Mkdir(dir).value();

        for (const auto & setting : cgroup.settings)
          write_settings(dir, setting).value();

        write_settings(dir, TASK).value();
      }
    else
      LOG_WARNING << "Controller " << cgroup.control << " is not support!!";

    return {};
  }

  std::expected<void, error::Err>
    CgroupsV1::setup(const config::Container_Options & config) noexcept
  {
    for (const auto & control : config.cgroups_options)
      write_contorl(config.hostname, control).value();

    LOG_INFO << "Setting cgroups by cgroups-v1...✓";
    return {};
  }

  std::expected<void, error::Err>
    CgroupsV1::clean_control_task(const config::CgroupsV1::Control & cgroup) noexcept
  {
    const std::string task = "/sys/fs/cgroup/" + cgroup.control + "/tasks";

    const int taskfd = unix::Filesystem::Open(task.c_str(), O_WRONLY)
                         .transform_error([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot open the cgroups tasks controller " + cgroup.control);
    }).value();

    unix::Filesystem::Write(taskfd, "0")
      .transform_error([&](const auto & e) {
      unix::Filesystem::Close(taskfd)
        .transform_error([&](const auto & e) {
        return ERR_MSG(
          error::Code::Cgroups,
          "Cannot close the cgroups tasks controller " + cgroup.control);
      }).value();

      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot write the cgroups tasks controller " + cgroup.control);
    }).value();

    unix::Filesystem::Close(taskfd)
      .transform_error([&](const auto & e) {
      return ERR_MSG(
        error::Code::Cgroups,
        "Cannot close the cgroups tasks controller " + cgroup.control);
    }).value();

    return {};
  }

  std::expected<void, error::Err>
    CgroupsV1::clean(const config::Container_Options & config) noexcept
  {
    for (const auto & cgroup : config.cgroups_options)
      {
        const std::string dir =
          "/sys/fs/cgroup/" + cgroup.control + "/" + config.hostname;

        clean_control_task(cgroup).value();

        unix::Filesystem::Rmdir(dir)
          .transform_error([&](const auto & _) {
          return ERR_MSG(
            error::Code::Cgroups, "Cannot clean cgroups controller " + cgroup.control);
        }).value();
      }

    LOG_INFO << "Cleaning cgroups-v1 settings...✓";
    return {};
  }

  std::expected<void, error::Err>
    Resource::clean(const config::Container_Options & config) noexcept
  {
    CgroupsV1::clean(config).value();

    return {};
  }
} // namespace bonding::resource
