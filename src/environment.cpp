#include "include/environment.h"
#include <cstring>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <utility>

namespace bonding::environment
{
  Result<std::pair<int, int>, error::Err>
  Kernel::parse_version(const utsname & host)
  {
    int major = 0, minor = 0;

    if (2 != sscanf(host.release, "%u.%u", &major, &minor))
      {
        return ERR_MSG(error::Code::CheckError,
                       "weird release format: " + std::string(host.release));
      }

    return Ok(std::make_pair(major, minor));
  }

  Result<std::map<std::string, bool>, error::Err>
  CgroupsV1::check_supported_controller() noexcept
  {
    std::map<std::string, bool> supported_controllers;

    for (const std::string controller_name : controllers)
      {
        std::string::size_type index = controller_name.find('.');

        if (std::string::npos == index)
          {
            const std::string controller_path = PATH + controller_name;
            if (-1 == access(controller_path.c_str(), F_OK))
              supported_controllers.insert(std::make_pair(controller_name, false));
            else
              supported_controllers.insert(std::make_pair(controller_name, true));
          }
        else
          {
            const std::string controller_path = PATH + controller_name.substr(0, index);
            supported_controllers.insert(std::make_pair(controller_name, false));

            DIR * d = opendir(controller_path.c_str());
            struct dirent * dir = NULL;
            if (NULL != d)
              {
                while (NULL != (dir = readdir(d)))
                  if (std::string::npos != std::string(dir->d_name).find(controller_name))
                    {
                      supported_controllers.insert_or_assign(controller_name, true);
                      break;
                    }

                closedir(d);
              }
          }
      }

    return Ok(supported_controllers);
  }
}