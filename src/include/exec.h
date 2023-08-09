/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef __BONDING_EXEC_H__
#define __BONDING_EXEC_H__

#include "error.h"
#include "result.hpp"
#include <vector>

namespace bonding::exec
{

  class Execve
  {
   public:
    /** The execve systemcall wrapper */
    static Result<Void, error::Err> call(const std::string & path,
                                         const std::vector<std::string> & argv) noexcept;

   private:
    inline static std::vector<char *> args;
  };

};

#endif /* __BONDING_EXEC_H__ */