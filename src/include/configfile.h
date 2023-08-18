#ifndef __BONDING_CONFIGFILE_H__
#define __BONDING_CONFIGFILE_H__

#include "config.h"
#include "configor/json.hpp"
#include "error.h"
#include "result.hpp"

namespace bonding::configfile
{
  class Config_File
  {
   public:
    static config::Container_Options read(const std::string file) noexcept;

   private:
    static Result<std::vector<std::pair<std::string, std::string>>, error::Err>
    read_mounts(const configor::json::value & data) noexcept;

    static Result<std::vector<std::pair<std::string, std::string>>, error::Err>
    read_cgroups_options(const configor::json::value & data) noexcept;

    static Result<std::vector<std::string>, Void>
    parse_argv(const std::string argv) noexcept;

    static Result<std::pair<int, int>, error::Err> generate_socketpair() noexcept;
  };
}

#endif /* __BONDING_CONFIGFILE_H__ */