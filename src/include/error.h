#ifndef __BONDING_ERROR_H__
#define __BONDING_ERROR_H__

#include "result.hpp"
#include <cerrno>
#include <spdlog/spdlog.h>
#include <string>

namespace bonding::error
{

  enum class Code
  {
    Undefined,
    SocketError,
    ChildProcessError,
    ContainerError
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

    int32_t to_exit_code() const noexcept;
    std::string to_string() const noexcept;

   private:
    const Code m_code;
    const int m_errno;
  };

  /** Get the result from a function, and exit the process with the correct error
   ** code. Linux executable returns a number when they exit, which describe how
   ** everything went a return code of 0 means that there was no errors, any
   ** other number describe an error and what that error is
   ** (based on the return code value). */
  void exit_with_return_code(Result<Unit, const Err> result) noexcept;

}

#endif /* __BONDING_ERROR_H__ */