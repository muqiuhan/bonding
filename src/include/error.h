/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_ERROR_H
#define BONDING_ERROR_H

#include "result.hpp"
#include <cerrno>
#include <exception>
#include <map>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <utility>

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
    Err() { std::terminate(); }

    Err(const Code code,
        std::string custom,
        const uint32_t line,
        std::string file,
        std::string function)
      : m_code(code)
      , m_errno(errno)
      , m_custom(std::move(custom))
      , m_line(line)
      , m_file(std::move(file))
      , m_function(std::move(function))
    {
      spdlog::error("{}", to_string(), function);
      spdlog::debug("In the `{}` function on line `{}` of the file `{}` ({}:{})",
                    function,
                    line,
                    file,
                    file,
                    line);
    }

    [[nodiscard]] static int32_t to_exit_code() noexcept;
    [[nodiscard]] std::string to_string() const noexcept;

   private:
    Code m_code;

    int m_errno;
    uint32_t m_line;

    std::string m_custom;
    std::string m_function;
    std::string m_file;
  };

}

#define ERR(CODE)                                                                        \
  result::Err(bonding::error::Err(CODE, "", __LINE__, __FILE__, __FUNCTION__))
#define ERR_MSG(CODE, MSG)                                                               \
  result::Err(bonding::error::Err(CODE, MSG, __LINE__, __FILE__, __FUNCTION__))

#endif /* BONDING_ERROR_H */
