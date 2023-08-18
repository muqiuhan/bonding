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
  };
}

#endif /* __BONDING_CONFIGFILE_H__ */