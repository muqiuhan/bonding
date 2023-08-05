#include "include/syscall.h"
#include <libseccomp/seccomp.h>

namespace bonding::syscall
{
  Result<Unit, error::Err>
  Syscall::setup() noexcept
  {
    spdlog::debug("Refusing / Filtering unwanted syscalls");

    /* Initialize seccomp profile with all syscalls allowed by default */
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (NULL == ctx)
      return Err(error::Err(error::Code::SystemcallError, "seccomp_init error"));

    if (0 != seccomp_load(ctx))
      return Err(error::Err(error::Code::SystemcallError, "seccomp_load error"));

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Syscall::clean() noexcept
  {
    seccomp_release(ctx);

    return Ok(Unit());
  }
}
