#ifndef BONDING_CONFIGFILE_H
#define BONDING_CONFIGFILE_H

#include "config.h"
#include "configor/json.hpp"
#include "error.h"
#include "result.hpp"

namespace bonding::configfile
{
  class Config_File
  {
   public:
    static config::Container_Options read(const std::string& file) noexcept;

   private:
    static Result<std::vector<std::pair<std::string, std::string>>, error::Err>
    read_mounts(const configor::json::value & data) noexcept;

    static Result<std::vector<std::pair<std::string, std::string>>, error::Err>
    read_cgroups_options(const configor::json::value & data) noexcept;

    static Result<std::vector<std::string>, Void>
    parse_argv(std::string argv) noexcept;

    static Result<std::pair<int, int>, error::Err> generate_socketpair() noexcept;
  };
}

#endif /* BONDING_CONFIGFILE_H */