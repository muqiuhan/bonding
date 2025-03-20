/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/child.h"
#include "include/capabilities.h"
#include "include/exec.h"
#include "include/hostname.h"
#include "include/log.hpp"
#include "include/mount.h"
#include "include/namespace.h"
#include "include/syscall.h"

#include <__expected/expected.h>
#include <cstdio>
#include <error.h>
#include <sched.h>
#include <sys/wait.h>

namespace bonding::child
{
  std::expected<void, error::Err> Child::Process::setup_container_configurations() noexcept
  {
    hostname::Hostname::setup(container_options->hostname).value();
    mounts::Mount::setup(
      container_options->mount_dir, container_options->hostname, container_options->mounts)
      .value();
    ns::Namespace::setup(container_options->ipc.second, container_options->uid).value();
    capabilities::Capabilities::setup().value();
    syscall::Syscall::setup().value();

    return {};
  }

  int Child::Process::_main(void *options) noexcept
  {
    container_options = static_cast<config::Container_Options *>(options);

    setup_container_configurations()
      .transform([]() -> std::expected<void, error::Err> {
      LOG_INFO << "Container setup successfully";
      return {};
    })
      .transform_error([&](const error::Err &err) {
      LOG_ERROR << "Error while creating container";
      return err;
    }).value();

    int ret_code = 0;

    if (!exec::Execve::call(container_options->path, container_options->argv).has_value())
      ret_code = -1;

    return ret_code;
  }

  std::expected<pid_t, error::Err> Child::generate_child_process(
    const config::Container_Options container_options) noexcept
  {
    const pid_t child_pid = clone(
      Process::_main,
      static_cast<char *>(Process::STACK) + Process::STACK_SIZE,
      container_options.clone_flags,
      (void *) &container_options);

    if (-1 == child_pid)
      return std::unexpected(ERR(error::Code::ChildProcess));

    return child_pid;
  }

  std::expected<void, error::Err> Child::wait() const noexcept
  {
    LOG_DEBUG << "Waiting for child process " << m_pid << " finish...";

    int child_process_status = 0;

    /** To wait for children produced by clone(), need __WCLONE flag */
    if (-1 == waitpid(m_pid, &child_process_status, __WALL))
      return std::unexpected(ERR(error::Code::Container));

    LOG_INFO << "Child process exit with code " << ((child_process_status >> 8) & 0xFF)
             << ", signal " << (child_process_status & 0x7F);

    return {};
  }
} // namespace bonding::child
