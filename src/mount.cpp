/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/mount.h"

#include <filesystem>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace bonding::mounts
{

  Result<Void, error::Err>
  Mount::_umount(const std::string & path) noexcept
  {
    spdlog::debug("Umount {}", path);
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        spdlog::error("Unable to umount {}", path);
        return ERR(error::Code::Mounts);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::_delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        spdlog::error("Unable to remove {}", path);
        return ERR(error::Code::Mounts);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::setup(
    const std::string & mount_dir,
    const std::string & hostname,
    const std::vector<std::pair<std::string, std::string>> & mounts_paths) noexcept
  {
    spdlog::info("Setting mount points...");
    _mount("", "/", MS_REC | MS_PRIVATE).unwrap();

    root = ".bonding/tmp/" + hostname + "/";
    const std::string old_root_tail = "oldroot." + hostname + "/";
    const std::string put_old = root + old_root_tail;

    _create(root).unwrap();
    _mount(mount_dir, root, MS_BIND | MS_PRIVATE).unwrap();
    _create(put_old).unwrap();

    for (const auto & [real_path, mount_path] : mounts_paths)
      {
        const std::string mount_dir = root + mount_path;
        _create(mount_dir).unwrap();
        _mount(real_path, mount_dir, MS_BIND | MS_PRIVATE).unwrap();
      }

    if (-1 == syscall(SYS_pivot_root, root.c_str(), put_old.c_str()))
      return ERR(error::Code::Mounts);

    const std::string old_root = "/" + old_root_tail;

    /* Ensure not inside the directory we want to umount. */
    if (-1 == chdir("/"))
      return ERR(error::Code::Mounts);

    _umount(old_root).unwrap();
    _delete(old_root).unwrap();
    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::clean() noexcept
  {
    if (-1 == rmdir(root.c_str()))
      return ERR(error::Code::Mounts);

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::_mount(const std::string & path,
                const std::string & mount_point,
                unsigned long flags) noexcept
  {
    spdlog::info("Mount {} to {}", path, mount_point);
    if (-1
        == mount((path.empty() ? nullptr : path.c_str()),
                 mount_point.c_str(),
                 nullptr,
                 flags,
                 nullptr))
      {
        spdlog::error("Cannot mount {} to {}", path, mount_point);
        return ERR(error::Code::Mounts);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::_create(const std::string & path) noexcept
  {
    spdlog::debug("Create {}", path);
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        spdlog::error("Cannot create direcotry {}");
        return ERR(error::Code::Mounts);
      }

    return Ok(Void());
  }
}
