#ifndef __BONDING_CONFIGFILE_H__
#define __BONDING_CONFIGFILE_H__

#include "config.h"
#include "error.h"
#include "result.hpp"

namespace bonding::configfile
{
  class Config_File
  {
   public:
    static cli::Args read(const std::string file) noexcept;

   private:
    static bool parse_debug(const auto & toml) noexcept;
    static std::string parse_command(const auto & toml) noexcept;
    static int parse_uid(const auto & toml) noexcept;
    static std::string parse_mount_dir(const auto & toml) noexcept;
    static std::string parse_hostname(const auto & toml) noexcept;
    static std::vector<std::pair<std::string, std::string>>
    parse_mounts(const auto & toml) noexcept;
  };
}

#endif /* __BONDING_CONFIGFILE_H__ */