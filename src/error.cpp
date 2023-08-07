#include "include/error.h"
#include <format>

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
        return std::format("Child Process Error: {}", strerror(m_errno));
      else if (m_code == Code::ContainerError)
        return std::format("Container Error: {}", strerror(m_errno));
      else if (m_code == Code::SocketError)
        return std::format("Socket Error: {}", strerror(m_errno));
      else if (m_code == Code::HostnameError)
        return std::format("Hostname Error: {}", strerror(m_errno));
      else if (m_code == Code::MountsError)
        return std::format("Mounts Error: {}", strerror(m_errno));
      else if (m_code == Code::NamespaceError)
        return std::format("Namespace Error: {}", strerror(m_errno));
      else if (m_code == Code::CgroupsError)
        return std::format("Cgroups Error: {}", strerror(m_errno));
      else
        return std::format("Undefined Error: {}", strerror(m_errno));
    };

    if (m_custom == "")
      return original_error();

    return std::format("{}\n\t\tCustom: {}", original_error(), m_custom);
  }

} // namespace bonding::error
