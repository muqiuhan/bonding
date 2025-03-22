/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/mount.h"

#include <filesystem>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace bonding::mounts
{

  std::expected<void, error::Err> Mount::_umount(const std::string & path) noexcept
  {
    LOG_DEBUG << "Umount " << path;
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        LOG_ERROR << "Unable to umount " << path;
        return std::unexpected(ERR(error::Code::Mounts));
      }

    return {};
  }

  std::expected<void, error::Err> Mount::_delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        LOG_ERROR << "Unable to remove " << path;
        return std::unexpected(ERR(error::Code::Mounts));
      }

    return {};
  }

  std::expected<void, error::Err> Mount::setup(
    const std::string &                                      mount_dir,
    const std::string &                                      hostname,
    const std::vector<std::pair<std::string, std::string>> & mounts_paths) noexcept
  {
    LOG_INFO << "Setting mount points...✓";
    _mount("", "/", MS_REC | MS_PRIVATE).value();

    root = ".bonding/tmp/" + hostname + "/";
    const std::string old_root_tail = "oldroot." + hostname + "/";
    const std::string put_old = root + old_root_tail;

    _create(root).value();
    _mount(mount_dir, root, MS_BIND | MS_PRIVATE).value();
    _create(put_old).value();

    for (const auto & [real_path, mount_path] : mounts_paths)
      {
        const std::string mount_dir = root + mount_path;
        _create(mount_dir).value();
        _mount(real_path, mount_dir, MS_BIND | MS_PRIVATE).value();
      }

    if (-1 == syscall(SYS_pivot_root, root.c_str(), put_old.c_str()))
      return std::unexpected(ERR(error::Code::Mounts));

    const std::string old_root = "/" + old_root_tail;

    /* Ensure not inside the directory we want to umount. */
    if (-1 == chdir("/"))
      return std::unexpected(ERR(error::Code::Mounts));

    _umount(old_root).value();
    _delete(old_root).value();

    return {};
  }

  std::expected<void, error::Err> Mount::clean() noexcept
  {
    if (-1 == rmdir(root.c_str()))
      return std::unexpected(ERR(error::Code::Mounts));

    return {};
  }

  std::expected<void, error::Err> Mount::_mount(
    const std::string & path,
    const std::string & mount_point,
    unsigned long       flags) noexcept
  {
    if (
      -1
      == mount(
        (path.empty() ? nullptr : path.c_str()),
        mount_point.c_str(),
        nullptr,
        flags,
        nullptr))
      return std::unexpected(
        ERR_MSG(error::Code::Mounts, "Cannot mount " + path + " to " + mount_point));

    if (!path.empty())
      {
        LOG_INFO << "Mount " << path << " to " << mount_point << "...✓";
      }

    return {};
  }

  std::expected<void, error::Err> Mount::_create(const std::string & path) noexcept
  {
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        return std::unexpected(
          ERR_MSG(error::Code::Mounts, "Cannot create direcotry " + path));
      }

    LOG_DEBUG << "Create " << path << "...✓";
    return {};
  }
} // namespace bonding::mounts
