#include "include/mount.h"

#include <error.h>
#include <filesystem>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace bonding::mounts
{

  Result<Void, error::Err>
  Mount::__umount(const std::string & path) noexcept
  {
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        spdlog::error("Unable to umount {}", path);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        spdlog::error("Unable to remove {}", path);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::setup(const std::string mount_dir, const std::string hostname) noexcept
  {
    spdlog::info("Setting mount points...");
    __mount("", "/", MS_REC | MS_PRIVATE).unwrap();

    root = ".bonding/tmp/bonding." + hostname + "/";
    const std::string old_root_tail = "bonding.oldroot." + hostname + "/";
    const std::string put_old = root + old_root_tail;

    __create(root).unwrap();
    __mount(mount_dir, root, MS_BIND | MS_PRIVATE).unwrap();
    __create(put_old).unwrap();

    if (-1 == syscall(SYS_pivot_root, root.c_str(), put_old.c_str()))
      return Err(bonding::error::Err(bonding::error::Code::MountsError));

    spdlog::info("Umounting old root...");
    const std::string old_root = "/" + old_root_tail;

    /* Ensure not inside the directory we want to umount. */
    if (-1 == chdir("/"))
      return Err(bonding::error::Err(bonding::error::Code::MountsError));

    __umount(old_root).unwrap();
    __delete(old_root).unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::clean() noexcept
  {
    spdlog::info("root = {}", root);
    if (-1 == rmdir(root.c_str()))
      return Err(error::Err(error::Code::MountsError));

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__mount(const std::string & path,
                 const std::string & mount_point,
                 unsigned long flags) noexcept
  {
    if (-1
        == mount((path == "" ? NULL : path.c_str()),
                 mount_point.c_str(),
                 NULL,
                 flags,
                 NULL))
      {
        spdlog::error("Cannot mount {} to {}", path, mount_point);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__create(const std::string & path) noexcept
  {
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (const std::filesystem::filesystem_error & err)
      {
        spdlog::error("Cannot create direcotry {}: {}", path, err.what());
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }
    catch (...)
      {
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Void());
  }
}
