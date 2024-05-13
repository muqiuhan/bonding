/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/error.h"

namespace bonding::error
{

  std::string Err::to_string() const noexcept
  {
    const std::string system_error_msg = strerror(m_errno);

    return CODE_TO_STRING.at(m_code) + ": "
           + (system_error_msg == "Success" ? "Unknown error" : system_error_msg)
           + (m_custom.empty() ? "" : "\n\t\t\t  > " + m_custom);
  }

} // namespace bonding::error
