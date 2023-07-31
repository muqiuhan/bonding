#include "include/child.h"

#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>

namespace bonding::child
{
  Result<Unit, error::Err>
  Child::Process::setup_container_configurations() noexcept
  {
    container_options.m_hostname.set();
    return Ok(Unit());
  }

  int
  Child::Process::__main(void * options) noexcept
  {
    spdlog::info("==========< Child Process Output >==========");

    container_options = *((bonding::config::Container_Options *)(options));
    setup_container_configurations()
      .and_then([](const Unit ok) {
        spdlog::info("Container setup successfully");
        return Ok(ok);
      })
      .or_else([](const error::Err err) {
        spdlog::error("Error while configuring container: {}", err.to_string());
        exit(-1);
        return Err(err);
      });

    spdlog::info("==========< Child Process Output >==========");
    return 0;
  }

  Result<pid_t, error::Err>
  Child::generate_child_process(
    const bonding::config::Container_Options container_options) noexcept
  {
    const pid_t child_pid = clone(Process::__main,
                                  Process::STACK + Process::STACK_SIZE,
                                  CLONE_NEWNS         /* new mount namespace */
                                    | CLONE_NEWCGROUP /* new cgroup namespace */
                                    | CLONE_NEWPID    /* new pid namespace */
                                    | CLONE_NEWIPC    /* new ipc namespace */
                                    | CLONE_NEWNET    /* new network namespace */
                                    | CLONE_NEWUTS    /* new uts namespace */
                                    | SIGCHLD,
                                  (void *)&container_options);

    if (-1 == child_pid)
      return Err(bonding::error::Err(bonding::error::Code::ChildProcessError));

    return Ok(child_pid);
  }

  Result<Unit, error::Err>
  Child::wait() const noexcept
  {
    spdlog::debug("Waiting for child process {} finish...", m_pid);

    int child_process_status = 0;

    /** To wait for children produced by clone(), need __WCLONE flag */
    if (-1 == waitpid(m_pid, &child_process_status, __WCLONE))
      return Err(bonding::error::Err(bonding::error::Code::ContainerError));

    spdlog::info("Child process exit with code {}, signal {}",
                 (child_process_status >> 8) & 0xFF,
                 child_process_status & 0x7F);

    return Ok(Unit());
  }
}