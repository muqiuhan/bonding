/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/mount.h"

#include <error.h>
#include <exception>
#include <filesystem>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace bonding::mounts
{

  Result<Void, error::Err>
  Mount::__umount(const std::string & path) noexcept
  {
    spdlog::debug("Umount {}", path);
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        spdlog::error("Unable to umount {}", path);
        return ERR(error::Code::MountsError);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        spdlog::error("Unable to remove {}", path);
        return ERR(error::Code::MountsError);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::setup(
    const std::string & mount_dir,
    const std::string & hostname,
    const bool random_hostname,
    const std::vector<std::pair<std::string, std::string>> & mounts_paths) noexcept
  {
    spdlog::info("Setting mount points...");
    __mount("", "/", MS_REC | MS_PRIVATE).unwrap();

    root = ".bonding/tmp/" + hostname + "/";
    const std::string old_root_tail = "oldroot." + hostname + "/";
    const std::string put_old = root + old_root_tail;

    __create(root).unwrap();
    __mount(mount_dir, root, MS_BIND | MS_PRIVATE).unwrap();
    __create(put_old).unwrap();

    for (const auto & [real_path, mount_path] : mounts_paths)
      {
        const std::string mount_dir = root + mount_path;
        __create(mount_dir).unwrap();
        __mount(real_path, mount_dir, MS_BIND | MS_PRIVATE).unwrap();
      }

    if (-1 == syscall(SYS_pivot_root, root.c_str(), put_old.c_str()))
      return ERR(error::Code::MountsError);

    const std::string old_root = "/" + old_root_tail;

    /* Ensure not inside the directory we want to umount. */
    if (-1 == chdir("/"))
      return ERR(error::Code::MountsError);

    __umount(old_root).unwrap();
    __delete(old_root).unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::clean() noexcept
  {
    if (-1 == rmdir(root.c_str()))
      return ERR(error::Code::MountsError);

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__mount(const std::string & path,
                 const std::string & mount_point,
                 unsigned long flags) noexcept
  {
    spdlog::info("Mount {} to {}", path, mount_point);
    if (-1
        == mount((path == "" ? NULL : path.c_str()),
                 mount_point.c_str(),
                 NULL,
                 flags,
                 NULL))
      {
        spdlog::error("Cannot mount {} to {}", path, mount_point);
        return ERR(error::Code::MountsError);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::__create(const std::string & path) noexcept
  {
    spdlog::debug("Create {}", path);
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        spdlog::error("Cannot create direcotry {}");
        return ERR(error::Code::MountsError);
      }

    return Ok(Void());
  }
}
