/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef __BONDING_ERROR_H__
#define __BONDING_ERROR_H__

#include "result.hpp"
#include <cerrno>
#include <exception>
#include <map>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace bonding::error
{
  enum class Code
  {
    Undefined,
    Socket,
    ChildProcess,
    Container,
    Hostname,
    Mounts,
    Namespace,
    Systemcall,
    Cgroups,
    Exec,
    Capabilities,
    Unix,
    Environment,
    Cli,
    Configfile,
  };

  inline const std::map<Code, std::string> CODE_TO_STRING = {
    { Code::Undefined, "Unknown Error" },
    { Code::Socket, "Socket Error" },
    { Code::ChildProcess, "Child Process Error" },
    { Code::Container, "Container Error" },
    { Code::Hostname, "Hostname Error" },
    { Code::Mounts, "Mounts Error" },
    { Code::Namespace, "Namespace Error" },
    { Code::Systemcall, "System Call Error" },
    { Code::Cgroups, "Cgroups Error" },
    { Code::Exec, "Execute Error" },
    { Code::Cli, "Cli Error" },
    { Code::Environment, "Envronment Error" },
    { Code::Capabilities, "Capabilities Error" },
    { Code::Unix, "Unix Error" },
    { Code::Configfile, "Config File Error" },
  };

  class Err
  {
   public:
    Err()
      : Err(Code::Undefined)
    {
    }

    Err(const Code code)
      : Err(code, "")
    {
      spdlog::error("{}", to_string());
    }

    Err(const Code code, const std::string custom)
      : Err(code, custom, 0, "Unknown", "Unknown")
    {
      spdlog::error("{}", to_string());
    }

    Err(const Code code,
        const std::string custom,
        const uint32_t line,
        const std::string file,
        const std::string function)
      : m_code(code)
      , m_errno(errno)
      , m_custom(std::move(custom))
      , m_line(line)
      , m_file(file)
      , m_function(function)
    {
      spdlog::error("{}", to_string(), function);
      spdlog::critical("In the `{}` function on line `{}` of the file `{}` ({}:{})",
                       function,
                       line,
                       file,
                       file,
                       line);
    }

    int32_t to_exit_code() const noexcept;
    std::string to_string() const noexcept;

   private:
    const Code m_code;

    const int m_errno;
    const uint32_t m_line;

    const std::string m_custom;
    const std::string m_function;
    const std::string m_file;
  };

  /** Get the result from a function, and exit the process with the correct error
   ** code. Linux executable returns a number when they exit, which describe how
   ** everything went a return code of 0 means that there was no errors, any
   ** other number describe an error and what that error is
   ** (based on the return code value). */
  void exit_with_return_code(Result<Void, const Err> result) noexcept;
}

#define ERR(CODE)                                                                        \
  result::Err(bonding::error::Err(CODE, "", __LINE__, __FILE__, __FUNCTION__))
#define ERR_MSG(CODE, MSG)                                                               \
  result::Err(bonding::error::Err(CODE, MSG, __LINE__, __FILE__, __FUNCTION__))

#endif /* __BONDING_ERROR_H__ */
