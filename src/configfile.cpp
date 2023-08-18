#include "include/configfile.h"
#include "configor/json.hpp"
#include "include/cli.h"
#include "include/unix.h"

namespace bonding::configfile
{
  cli::Args
  Config_File::read(const std::string path) noexcept
  {
    const configor::json::value data =
      configor::json::parse(unix::Filesystem::read_entire_file(path).unwrap());

    std::vector<std::pair<std::string, std::string>> mounts;

    for (auto && mount : data["mounts"])
      mounts.push_back(std::make_pair(mount[0], mount[1]));

    return cli::Args{ data["debug"],     data["command"],  data["uid"],
                      data["mount_dir"], data["hostname"], mounts };
  }

}