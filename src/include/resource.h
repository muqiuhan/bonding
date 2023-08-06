#ifndef __BONDING_RESOURCE_H__
#define __BONDING_RESOURCE_H__

#include "error.h"
#include "result.hpp"
#include <cstdint>

namespace bonding::resource
{
  class Resource
  {
   public:
    static Result<Void, error::Err> setup(const std::string hostname) noexcept;

   private:
    inline static const int64_t KMEM_LIMIT = 1024 * 1024 * 1024;
    inline static const int64_t MEM_LIMIT = KMEM_LIMIT;
    inline static const int64_t MAX_PID = 64;
    inline static const uint64_t NOFILE_RLIMIT = 64;
  };
};

#endif /* __BONDING_RESOURCE_H__ */
