#include "include/child.h"

#define _GNU_SOURCE
#include <sched.h>

namespace bonding::child
{

  int
  Child::Process::__main(void * options) noexcept
  {
    container_options = *((bonding::config::Container_Options *)(options));
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
}