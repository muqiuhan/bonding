/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/error.h"

namespace bonding::error
{

  void
  exit_with_return_code(Result<Void, const Err> result) noexcept
  {
    if (result.is_ok())
      {
        spdlog::info("Exit without any error, returning 0");
        exit(0);
      }
    else
      {
        const Err err = result.unwrap_err();
        const int32_t return_code = err.to_exit_code();

        spdlog::error("Error on exit:\n\t{}\n\tReturning: {}",
                      err.to_string(),
                      return_code);

        exit(return_code);
      }
  }

  int32_t
  Err::to_exit_code() const noexcept
  {
    return 1;
  }

  std::string
  Err::to_string() const noexcept
  {
    const auto original_error = [&]() {
      if (m_code == Code::ChildProcessError)
        return "Child Process Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::ContainerError)
        return "Container Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::SocketError)
        return "Socket Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::HostnameError)
        return "Hostname Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::MountsError)
        return "Mounts Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::NamespaceError)
        return "Namespace Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::CgroupsError)
        return "Cgroups Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::ExecError)
        return "Exec Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::CapabilitiesError)
        return "Capabilities Error: " + std::string(strerror(m_errno));
      else if (m_code == Code::UnixError)
        return "Unix Error: " + std::string(strerror(m_errno));
      else
        return "Undefined Error: " + std::string(strerror(m_errno));
    };

    if (m_custom == "")
      return original_error();

    return original_error() + "\n\t\t  Help: " + m_custom;
  }

} // namespace bonding::error
