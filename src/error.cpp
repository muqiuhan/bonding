/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/error.h"

namespace bonding::error
{

  std::string
  Err::to_string() const noexcept
  {
    if (m_custom.empty())
      return CODE_TO_STRING.at(m_code) + ": " + strerror(m_errno);

    return CODE_TO_STRING.at(m_code) + ": " + strerror(m_errno) + "\n\t\t> " + m_custom;
  }

} // namespace bonding::error
