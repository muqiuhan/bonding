/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/config.h"

#include <iterator>
#include <sstream>

namespace bonding::config
{
  Result<std::vector<std::string>, Void>
  Container_Options::parse_argv(const std::string argv) noexcept
  {
    std::istringstream argv_stream(argv);
    const auto result =
      std::vector<std::string>((std::istream_iterator<std::string>(argv_stream)),
                               std::istream_iterator<std::string>());
    return Ok(result);
  }
}
