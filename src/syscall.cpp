#include "include/syscall.h"
#include <libseccomp/seccomp.h>

namespace bonding::syscall
{
  Result<Unit, error::Err>
  Syscall::setup() const noexcept
  {
    spdlog::debug("Refusing / Filtering unwanted syscalls");

    /* Initialize seccomp profile with all syscalls allowed by default */
    const scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (NULL == ctx)
      return Err(error::Err(error::Code::SystemcallError, "seccomp_init error"));

    if (0 != seccomp_load(ctx))
      return Err(error::Err(error::Code::SystemcallError, "seccomp_load error"));

    return Ok(Unit());
  }
}
