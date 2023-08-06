#include "include/syscall.h"
#include <algorithm>
#include <array>
#include <asm-generic/errno-base.h>
#include <libseccomp/seccomp.h>

namespace bonding::syscall
{
  Result<Unit, error::Err>
  Syscall::refuse_syscall(scmp_filter_ctx & ctx, const int syscall) noexcept
  {
    if (0 != seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), syscall, 0))
      return Err(error::Err(error::Code::SystemcallError, "seccomp_rule_add error"));

    return Ok(Unit());
  }

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

    const std::array<int, 10> refuse_syscalls = {
      SCMP_SYS(keyctl),         SCMP_SYS(add_key),       SCMP_SYS(request_key),
      SCMP_SYS(mbind),          SCMP_SYS(migrate_pages), SCMP_SYS(move_pages),
      SCMP_SYS(move_pages),     SCMP_SYS(set_mempolicy), SCMP_SYS(userfaultfd),
      SCMP_SYS(perf_event_open)
    };

    for (const int syscall : refuse_syscalls)
      refuse_syscall(ctx, syscall).unwrap();

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Syscall::clean() noexcept
  {
    seccomp_release(ctx);

    return Ok(Unit());
  }
}
