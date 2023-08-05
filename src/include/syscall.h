#ifndef __BONDING_SYSCALL_H__
#define __BONDING_SYSCALL_H__

#include "result.hpp"
#include "error.h"

namespace bonding::syscall
{
  class Syscall
  {
  public:
    Result<Unit, error::Err> setup() const noexcept;
  };
}

#endif /* __BONDING_SYSTEMCALL_H__ */
