/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/child.h"
#include "include/capabilities.h"
#include "include/container.h"
#include "include/exec.h"
#include "include/hostname.h"
#include "include/mount.h"
#include "include/namespace.h"
#include "include/syscall.h"
#include "spdlog/spdlog.h"

#include <cstdio>
#include <sched.h>
#include <sys/wait.h>

namespace bonding::child
{
  Result<Void, error::Err>
  Child::Process::setup_container_configurations() noexcept
  {
    hostname::Hostname::setup(container_options->hostname).unwrap();
    mounts::Mount::setup(container_options->mount_dir,
                         container_options->hostname,
                         container_options->mounts)
      .unwrap();
    ns::Namespace::setup(container_options->ipc.first, container_options->uid).unwrap();
    capabilities::Capabilities::setup().unwrap();
    syscall::Syscall::setup().unwrap();

    return Ok(Void());
  }

  int
  Child::Process::__main(void * options) noexcept
  {
    container_options = static_cast<config::Container_Options *>(options);

    setup_container_configurations()
      .and_then([](const Void ok) {
        spdlog::info("Container setup successfully");
        return Ok(ok);
      })
      .or_else([](const error::Err err) {
        spdlog::error("Error while creating container");
        return Err(err);
      })
      .unwrap();

    int ret_code = 0;
    if (exec::Execve::call(container_options->path, container_options->argv).is_err())
      ret_code = -1;

    return ret_code;
  }

  Result<pid_t, error::Err>
  Child::generate_child_process(
    const config::Container_Options container_options) noexcept
  {
    const pid_t child_pid =
      clone(Process::__main,
            static_cast<char *>(Process::STACK) + Process::STACK_SIZE,
            CLONE_NEWNS         /* new mount namespace */
              | CLONE_NEWCGROUP /* new cgroup namespace */
              | CLONE_NEWPID    /* new pid namespace */
              | CLONE_NEWIPC    /* new ipc namespace */
              | CLONE_NEWNET    /* new network namespace */
              | CLONE_NEWUTS /* new uts namespace */,
            (void *)&container_options);

    if (-1 == child_pid)
      return ERR(error::Code::ChildProcess);

    return Ok(child_pid);
  }

  Result<Void, error::Err>
  Child::wait() const noexcept
  {
    spdlog::debug("Waiting for child process {} finish...", m_pid);

    int child_process_status = 0;

    /** To wait for children produced by clone(), need __WCLONE flag */
    if (-1 == waitpid(m_pid, &child_process_status, __WALL))
      return ERR(error::Code::Container);

    spdlog::info("Child process exit with code {}, signal {}",
                 (child_process_status >> 8) & 0xFF,
                 child_process_status & 0x7F);

    return Ok(Void());
  }
}
