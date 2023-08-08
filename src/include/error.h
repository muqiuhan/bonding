#ifndef __BONDING_ERROR_H__
#define __BONDING_ERROR_H__

#include "result.hpp"
#include <cerrno>
#include <exception>
#include <spdlog/spdlog.h>
#include <string>

namespace bonding::error
{

  enum class Code
  {
    Undefined,
    SocketError,
    ChildProcessError,
    ContainerError,
    HostnameError,
    MountsError,
    NamespaceError,
    SystemcallError,
    CgroupsError,
    ExecError
  };

  class Err
  {
   public:
    Err()
      : Err(Code::Undefined)
    {
    }

    Err(const Code code)
      : m_code(code)
      , m_errno(errno)
    {
      spdlog::error("{}", to_string());
    }

    Err(const Code code, const std::string custom)
      : m_code(code)
      , m_errno(errno)
      , m_custom(std::move(custom))
    {
      spdlog::error("{}", to_string());
    }

    int32_t to_exit_code() const noexcept;
    std::string to_string() const noexcept;

   private:
    const Code m_code;
    const int m_errno;
    const std::string m_custom;
  };

  /** Get the result from a function, and exit the process with the correct error
   ** code. Linux executable returns a number when they exit, which describe how
   ** everything went a return code of 0 means that there was no errors, any
   ** other number describe an error and what that error is
   ** (based on the return code value). */
  void exit_with_return_code(Result<Void, const Err> result) noexcept;

}

#endif /* __BONDING_ERROR_H__ */
