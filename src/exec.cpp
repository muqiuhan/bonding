/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/exec.h"
#include "spdlog/fmt/bundled/format.h"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <unistd.h>

namespace bonding::exec
{
  Result<Void, error::Err>
  Execve::call(const std::string & path, const std::vector<std::string> & argv) noexcept
  {
    std::transform(
      argv.begin(),
      argv.end(),
      std::back_inserter(args),
      ([](const std::string & arg) { return const_cast<char *>(arg.c_str()); }));

    const auto x = &args[0];
    if (-1 == execve(path.c_str(), static_cast<char * const *>(&args[0]), NULL))
      return ERR(error::Code::Exec);

    return Ok(Void());
  }
}