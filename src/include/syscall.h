#ifndef __BONDING_SYSCALL_H__
#define __BONDING_SYSCALL_H__

#include "error.h"
#include "result.hpp"
#include <libseccomp/seccomp.h>

namespace bonding::syscall
{
  class Syscall
  {
   public:
    static Result<Unit, error::Err> setup() noexcept;
    static Result<Unit, error::Err> clean() noexcept;

   private:
    /** Totally deny any attempt to call that syscall in the child process. */
    static Result<Unit, error::Err> refuse_syscall(scmp_filter_ctx & ctx,
                                                   const int syscall) noexcept;

   private:
    inline static scmp_filter_ctx ctx = NULL;
  };
}

#endif /* __BONDING_SYSTEMCALL_H__ */
