#include "include/child.h"

#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>

namespace bonding::child
{

  int
  Child::Process::__main(void * options) noexcept
  {
    spdlog::info("==========< Child Process Output >==========");

    container_options = *((bonding::config::Container_Options *)(options));

    spdlog::info("==========< Child Process Output >==========");
    return 0;
  }

  Result<pid_t, error::Err>
  Child::generate_child_process(
    const bonding::config::Container_Options container_options) noexcept
  {
    uint8_t temp_stack[STACK_SIZE];

    const pid_t child_pid = clone(Process::__main,
                                  &temp_stack,
                                  CLONE_NEWNS         /* new mount namespace */
                                    | CLONE_NEWCGROUP /* new cgroup namespace */
                                    | CLONE_NEWPID    /* new pid namespace */
                                    | CLONE_NEWIPC    /* new ipc namespace */
                                    | CLONE_NEWNET    /* new network namespace */
                                    | CLONE_NEWUTS /* new uts namespace */,
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

    if (-1 == waitpid(m_pid, &child_process_status, WUNTRACED | WCONTINUED))
      return Err(bonding::error::Err(bonding::error::Code::ContainerError));

    spdlog::info("Child process exit with code {}, signal {}",
                 (child_process_status >> 8) & 0xFF,
                 child_process_status & 0x7F);
  }
}