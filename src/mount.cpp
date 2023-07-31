#include "include/mount.h"
#include "hostname.h"
#include <sys/mount.h>
#include <sys/stat.h>

namespace bonding::mounts
{

  Result<Unit, error::Err>
  Mount::set() const noexcept
  {
    spdlog::info("Setting mount points...");
    const std::string new_root = "/tmp/" + bonding::hostname::Xorshift::generate(10);

    __create(new_root).unwrap();
    __mount(m_mount_dir, "/", MS_PRIVATE).unwrap();

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
  __create(const std::string & path) noexcept
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
  }
}