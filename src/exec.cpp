/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/exec.h"
#include <__expected/unexpected.h>
#include <algorithm>
#include <iterator>
#include <unistd.h>
#include "include/error.h"

namespace bonding::exec
{
  std::expected<void, error::Err>
    Execve::call(const std::string &path, const std::vector<std::string> &argv) noexcept
  {
    std::transform(
      argv.begin(), argv.end(), std::back_inserter(args), ([](const std::string &arg) {
      return const_cast<char *>(arg.c_str());
    }));

    // const auto x = &args[0];
    if (-1 == execve(path.c_str(), static_cast<char *const *>(&args[0]), nullptr))
      return std::unexpected(ERR(error::Code::Exec));
    return {};
  }
} // namespace bonding::exec