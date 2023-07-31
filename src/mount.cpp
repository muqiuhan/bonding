#include "include/mount.h"
#include "include/hostname.h"

#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace bonding::mounts
{

  Result<Unit, error::Err>
  Mount::__umount(const std::string & path) noexcept
  {
    if (-1 == umount2(path.c_str(), MNT_DETACH))
      {
        spdlog::error("Unable to umount {}", path);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::__delete(const std::string & path) noexcept
  {
    if (-1 == remove(path.c_str()))
      {
        spdlog::error("Unable to remove {}", path);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::set() const noexcept
  {
    spdlog::info("Setting mount points...");
    const std::string new_root =
      "/tmp/bonding." + bonding::hostname::Xorshift::generate(10) + "/";
    const std::string old_root_tail =
      "bonding.oldroot." + bonding::hostname::Xorshift::generate(10) + "/";
    const std::string put_old = new_root + old_root_tail;

    __create(new_root).unwrap();
    __mount(m_mount_dir, "/", MS_PRIVATE).unwrap();
    __create(put_old).unwrap();

    if (-1 == syscall(SYS_pivot_root, new_root.c_str(), put_old.c_str()))
      return Err(bonding::error::Err(bonding::error::Code::MountsError));

    spdlog::info("Umounting old root...");
    const std::string old_root = "/" + old_root_tail;

    /* Ensure not inside the directory we want to umount. */
    if (-1 == chdir("/"))
      return Err(bonding::error::Err(bonding::error::Code::MountsError));

    __umount(old_root).unwrap();
    __delete(old_root).unwrap();

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::clean() const noexcept
  {
    return Ok(Unit());
  }

  Result<Unit, error::Err>
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
        spdlog::error("Cannot remount {}", mount_point);
        return Err(bonding::error::Err(bonding::error::Code::MountsError));
      }

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Mount::__create(const std::string & path) noexcept
  {
    const char sep = '/';
    const char * p;
    char * temp;
    bool ret = true;

    temp = (char *)calloc(1, strlen(path.c_str()) + 1);
    p = path.c_str();

    while ((p = strchr(p, sep)) != NULL)
      {
        /* Skip empty elements. Could be a Windows UNC path or
           just multiple separators which is okay. */
        if (p != path.c_str() && *(p - 1) == sep)
          {
            p++;
            continue;
          }

        /* Put the path up to this point into a temporary to
           pass to the make directory function. */
        memcpy(temp, path.c_str(), p - path.c_str());
        temp[p - path.c_str()] = '\0';
        p++;

        if (-1 == mkdir(temp, 0774))
          return Err(bonding::error::Err(bonding::error::Code::CreateDirectoryError));
      }

    free(temp);
    return Ok(Unit());
  }
}