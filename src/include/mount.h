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
    explicit Mount(const std::string mount_dir)
      : m_mount_dir(std::move(mount_dir))
    {
    }

    /** Mount user-provided m_mount_dir to
     ** the mountpoint /tmp/bonding.<random_letters> */
    Result<Unit, error::Err> set() const noexcept;

    Result<Unit, error::Err> clean() const noexcept;

   private:
    const std::string m_mount_dir;

   private:
    /** Call the mount() system call */
    static Result<Unit, error::Err> __mount(const std::string & path,
                                            const std::string & mount_point,
                                            unsigned long flags) noexcept;

    /** Create directories recursively based on path */
    static Result<Unit, error::Err> __create(const std::string & path) noexcept;

    /** Achieve isolation with the host system, the
     ** “old root” has to be unmounted so the contained
     ** application cannot access to the whole filesystem.*/
    static Result<Unit, error::Err> __umount(const std::string & path) noexcept;
    static Result<Unit, error::Err> __delete(const std::string & path) noexcept;
  };
};

#endif /* __BONDING_MOUNT_H__ */