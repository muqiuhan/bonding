/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_MOUNT_H
#define BONDING_MOUNT_H

#include "error.h"
#include <expected>

namespace bonding::mounts
{
  /** Remount the root / of filesystem with the MS_PRIVATE
   ** flag which will prevent any mount operation to be propagated. */
  class Mount
  {
  public:
    /** Mount user-provided m_mount_dir to
     ** the mountpoint /tmp/bonding.<random_letters> */
    static std::expected<void, error::Err> setup(
      const std::string &                                      mount_dir,
      const std::string &                                      hostname,
      const std::vector<std::pair<std::string, std::string>> & mounts_paths) noexcept;

    static std::expected<void, error::Err> clean() noexcept;

  private:
    /** Call the mount() system call */
    static std::expected<void, error::Err> _mount(
      const std::string & path,
      const std::string & mount_point,
      unsigned long       flags) noexcept;

    /** Create directories recursively based on path */
    static std::expected<void, error::Err> _create(const std::string & path) noexcept;

    /** Achieve isolation with the host system, the
     ** “old root” has to be unmounted so the contained
     ** application cannot access to the whole filesystem.*/
    static std::expected<void, error::Err> _umount(const std::string & path) noexcept;
    static std::expected<void, error::Err> _delete(const std::string & path) noexcept;

  private:
    inline static std::string root;
  };
}; // namespace bonding::mounts

#endif /* BONDING_MOUNT_H */
