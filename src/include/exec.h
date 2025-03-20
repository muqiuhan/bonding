/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_EXEC_H
#define BONDING_EXEC_H

#include "error.h"
#include <expected>
#include <vector>

namespace bonding::exec
{

  class Execve
  {
  public:
    /** The execve systemcall wrapper */
    static std::expected<void, error::Err>
      call(const std::string &path, const std::vector<std::string> &argv) noexcept;

  private:
    inline static std::vector<char *> args;
  };

}; // namespace bonding::exec

#endif /* BONDING_EXEC_H */