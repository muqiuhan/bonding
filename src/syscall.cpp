/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/syscall.h"
#include <algorithm>
#include <asm-generic/errno-base.h>

#if __has_include(<libseccomp/seccomp.h>)
#include <libseccomp/seccomp.h>
#else
#include <seccomp.h>
#endif

namespace bonding::syscall
{
  std::expected<void, error::Err> Syscall::refuse_if_comp() noexcept
  {
    for (const auto & [syscall, ind, biteq] : default_refuse_if_comp_syscalls)
      {
        const scmp_arg_cmp cmp = {ind, SCMP_CMP_MASKED_EQ, biteq, biteq};

        if (0 != seccomp_rule_add_array(ctx, SCMP_ACT_ERRNO(EPERM), syscall, 1, &cmp))
          return std::unexpected(
            ERR_MSG(error::Code::Systemcall, "seccomp_rule_add_array error"));
      }

    return {};
  }

  std::expected<void, error::Err> Syscall::refuse_syscall() noexcept
  {
    for (const int syscall : default_refuse_syscalls)
      if (0 != seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), syscall, 0))
        return std::unexpected(
          ERR_MSG(error::Code::Systemcall, "seccomp_rule_add error"));

    return {};
  }

  std::expected<void, error::Err> Syscall::setup() noexcept
  {
    /* Initialize seccomp profile with all syscalls allowed by default */
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (nullptr == ctx)
      return std::unexpected(ERR_MSG(error::Code::Systemcall, "seccomp_init error"));

    refuse_syscall();
    refuse_if_comp();

    if (0 != seccomp_load(ctx))
      return std::unexpected(ERR_MSG(error::Code::Systemcall, "seccomp_load error"));

    LOG_INFO << "Refusing / Filtering unwanted syscalls...✓";
    return {};
  }

  std::expected<void, error::Err> Syscall::clean() noexcept
  {
    seccomp_release(ctx);
    return {};
  }
} // namespace bonding::syscall
