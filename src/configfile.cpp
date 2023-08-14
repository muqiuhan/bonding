#include "include/configfile.h"
#include "include/cli.h"
#include "toml/get.hpp"
#include "toml/parser.hpp"
#include <toml.hpp>

namespace bonding::configfile
{
  cli::Args
  Config_File::read(const std::string file) noexcept
  {
    const auto data = toml::parse(file);

    return cli::Args{
      parse_debug(data),     parse_command(data),  parse_uid(data),
      parse_mount_dir(data), parse_hostname(data), parse_mounts(data),
    };
  }

  bool
  Config_File::parse_debug(const auto & toml) noexcept
  {
    return toml::find<bool>(toml, "debug");
  }

  std::string
  Config_File::parse_command(const auto & toml) noexcept
  {
    return toml::find<std::string>(toml, "command");
  }

  int
  Config_File::parse_uid(const auto & toml) noexcept
  {
    return toml::find<int>(toml, "uid");
  }

  std::string
  Config_File::parse_mount_dir(const auto & toml) noexcept
  {
    return toml::find<std::string>(toml, "mount_dir");
  }

  std::string
  Config_File::parse_hostname(const auto & toml) noexcept
  {
    return toml::find<std::string>(toml, "hostname");
  }

  std::vector<std::pair<std::string, std::string>>
  Config_File::parse_mounts(const auto & toml) noexcept
  {
    return toml::find<std::vector<std::pair<std::string, std::string>>>(toml, "mounts");
  }

}