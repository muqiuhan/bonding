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
    LOG_DEBUG << "Umount " << path;
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        LOG_ERROR << "Unable to umount " << path;
        return ERR(error::Code::Mounts);
      }

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::_delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        LOG_ERROR << "Unable to remove " << path;
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
    LOG_INFO << "Setting mount points...✓";
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
    if (-1
        == mount((path.empty() ? nullptr : path.c_str()),
                 mount_point.c_str(),
                 nullptr,
                 flags,
                 nullptr))
      return ERR_MSG(error::Code::Mounts, "Cannot mount " + path + " to " + mount_point);

    if (!path.empty())
      LOG_INFO << "Mount " << path << " to " << mount_point << "...✓";

    return Ok(Void());
  }

  Result<Void, error::Err>
  Mount::_create(const std::string & path) noexcept
  {
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        return ERR_MSG(error::Code::Mounts, "Cannot create direcotry " + path);
      }

    LOG_DEBUG << "Create " << path << "...✓";
    return Ok(Void());
  }
}
