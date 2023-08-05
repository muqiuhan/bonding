#include "include/syscall.h"

namespace bonding::syscall {
  Result<Unit, error::Err>
  Syscall::setup() const noexcept
  {
    spdlog::debug("Refusing / Filtering unwanted syscalls");
    
  }
}
