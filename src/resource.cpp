/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/resource.h"
#include "include/environment.h"
#include "include/unix.h"
#include "spdlog/spdlog.h"
#include <error.h>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

namespace bonding::resource
{
  Result<Void, error::Err>
  Resource::setup(const std::string hostname) noexcept
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
      return ERR(error::Code::CgroupsError);

    return Ok(Void());
  }

  Result<Void, error::Err>
  CgroupsV1::setup(const std::string hostname) noexcept
  {
    spdlog::debug("Setting cgroups by cgroups-v1...");

    for (const Control & cgroup : CONFIG)
      {
        if (environment::CgroupsV1::supported_controllers.contains(cgroup.control))
          {
            const std::string dir = "/sys/fs/cgroup/" + cgroup.control + "/" + hostname;
            unix::Filesystem::mkdir(dir).unwrap();

            for (const Control::Setting & setting : cgroup.settings)
              {
                const std::string path = dir + "/" + setting.name;

                int fd = open(path.c_str(), O_WRONLY);
                if (-1 == fd)
                  {
                    spdlog::warn("Option {} is not support", setting.name);
                    close(fd);
                    break;
                  }

                if (-1 == write(fd, setting.value.c_str(), setting.value.length()))
                  return ERR(error::Code::CgroupsError);

                close(fd);
              }
          }
        else
          spdlog::warn("Controller {} is not support", cgroup.control);
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

  Result<std::vector<CgroupsV1::Control>, error::Err>
  CgroupsV1::default_config() noexcept
  {
    return Ok(std::vector<CgroupsV1::Control>{ (Control{ .control = "memory", 
                    .settings = { (Control::Setting{ .name = "memory.limit_in_bytes",
						        .value = MEM_LIMIT }),
			TASK } }),

        (Control { .control = "cpu",
			    .settings =
			    {
			      (Control::Setting { 
                                .name = "cpu.shares", .value = CPU_SHARES }),
			      TASK,
			    } }),

        (Control { .control = "pids",
			    .settings =
			    {
			      (Control::Setting { .name = "pids.max",
							   .value = PIDS_MAX }),
			    TASK} }),
        (Control { .control = "blkio",
			    .settings = {
			      ( Control::Setting {
				.name = "blkio.bfq.weight", .value = PIDS_MAX }),
			      TASK} })});
  }

#ifdef __WITH_LIBCGROUP
  Result<std::vector<Cgroups::Control>, error::Err>
  Cgroups::default_config() noexcept
  {
    return Ok(std::vector<Control>{ (Control{ .control = "memory", 
                    .settings = { (Control::Setting{ .name = "memory.limit_in_bytes",
						        .value = MEM_LIMIT }),
			TASK } }),

        (Control { .control = "cpu",
			    .settings =
			    {
			      (Control::Setting { 
                                .name = "cpu.shares", .value = CPU_SHARES }),
			      TASK,
			    } }),

        (Control { .control = "pids",
			    .settings =
			    {
			      (Control::Setting { .name = "pids.max",
							   .value = PIDS_MAX }),
			    TASK} }),
        (Control { .control = "blkio",
			    .settings = {
			      ( Control::Setting {
				.name = "你写你妈了个逼", .value = PIDS_MAX }),
			      TASK} })});
  }

  Result<Void, error::Err>
  Cgroups::setup(const std::string hostname) noexcept
  {
    spdlog::debug("Setting cgroups via libcgroup...");

    CGROUP = cgroup_new_cgroup(hostname.c_str());

    if (NULL == CGROUP)
      return ERR(error::Code::CgroupsError);

    for (const Control & config : CONFIG)
      {
        cgroup_controller * controller =
          cgroup_add_controller(CGROUP, config.control.c_str());

        if (NULL == controller)
          return ERR(error::Code::CgroupsError);

        for (const Control::Setting & setting : config.settings)
          {

            const int res = cgroup_set_value_string(controller,
                                                    setting.name.c_str(),
                                                    setting.value.c_str());
            spdlog::debug("{} -> {} return {}", setting.value, setting.name, res);
            if (-1 == res)
              return ERR(error::Code::CgroupsError);
          }
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Cgroups::clean(const std::string hostname) noexcept
  {
    cgroup_free_controllers(CGROUP);
    return Ok(Void());
  }
#endif
}
