#ifndef __BONDING_MOUNT_H__
#define __BONDING_MOUNT_H__

#include "error.h"
#include "result.hpp"

namespace bonding::mounts
{
  /** Remount the root / of filesystem with the MS_PRIVATE
   ** flag which will prevent any mount operation to be propagated. */
  class Mount
  {
   public:
    static Result<Unit, error::Err> set() noexcept;
    static Result<Unit, error::Err> clean() noexcept;

   private:
    /** Call the mount() system call */
    static Result<Unit, error::Err> __mount_directory(const std::string & mount_point,
                                                      unsigned long flags) noexcept;
  };
};

#endif /* __BONDING_MOUNT_H__ */