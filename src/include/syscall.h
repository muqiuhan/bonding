/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_SYSCALL_H
#define BONDING_SYSCALL_H

#include "error.h"
#include "result.hpp"
#include <array>
#include <asm-generic/ioctls.h>
#include <cstdint>
#include <fcntl.h>
#include <sched.h>
#include <tuple>

#if __has_include(<libseccomp/seccomp.h>)
#include <libseccomp/seccomp.h>
#else
#include <seccomp.h>
#endif

namespace bonding::syscall
{
  /** Restrict system call by seccomp,
   ** seccomp (short for secure computing mode) is a computer security facility in the
   ** Linux kernel. */
  class Syscall
  {
   public:
    static Result<Void, error::Err> setup() noexcept;
    static Result<Void, error::Err> clean() noexcept;

   private:
    /** Totally deny any attempt to call that syscall in the child process. */
    static Result<Void, error::Err> refuse_syscall() noexcept;

    /** Takes a value and return wether the permission should be set or not. */
    static Result<Void, error::Err> refuse_if_comp() noexcept;

   private:
    inline static scmp_filter_ctx ctx = nullptr;

    inline static const std::array<int, 10> default_refuse_syscalls = {
      SCMP_SYS(keyctl),         SCMP_SYS(add_key),       SCMP_SYS(request_key),
      SCMP_SYS(mbind),          SCMP_SYS(migrate_pages), SCMP_SYS(move_pages),
      SCMP_SYS(move_pages),     SCMP_SYS(set_mempolicy), SCMP_SYS(userfaultfd),
      SCMP_SYS(perf_event_open)
    };

    /** Syscalls can be restricted when a particular condition is met. */
    inline static const std::array<std::tuple<int, unsigned int, scmp_datum_t>, 9>
      default_refuse_if_comp_syscalls = {
        std::make_tuple(SCMP_SYS(chmod), 1, S_ISUID),
        std::make_tuple(SCMP_SYS(chmod), 1, S_ISGID),
        std::make_tuple(SCMP_SYS(fchmod), 1, S_ISGID),
        std::make_tuple(SCMP_SYS(fchmod), 1, S_ISGID),
        std::make_tuple(SCMP_SYS(fchmodat), 2, S_ISGID),
        std::make_tuple(SCMP_SYS(fchmodat), 2, S_ISGID),
        std::make_tuple(SCMP_SYS(unshare), 0, CLONE_NEWUSER),
        std::make_tuple(SCMP_SYS(clone), 0, CLONE_NEWUSER),
        std::make_tuple(SCMP_SYS(ioctl), 1, TIOCSTI),
      };
  };
}

#endif /* BONDING_SYSTEMCALL_H */
