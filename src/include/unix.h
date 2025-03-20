/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_UNIX_H
#define BONDING_UNIX_H

#include "error.h"
#include <linux/prctl.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/utsname.h>
#include <expected>

/** Auto generate wrapper function for system calls function  */
#define GENERATE_SYSTEM_CALL_WRAPPER(                                                    \
  OK_TYPE, FAILURE_VALUE, WRAPPER_FUNCTION_SIGNATURE, SYSTEM_CALL_FUNCTION_NAME, ...)    \
  std::expected<OK_TYPE, error::Err> WRAPPER_FUNCTION_SIGNATURE noexcept                 \
  {                                                                                      \
    OK_TYPE SYSTEM_CALL_FUNCTION_RESULT = SYSTEM_CALL_FUNCTION_NAME(__VA_ARGS__);        \
    if (FAILURE_VALUE == SYSTEM_CALL_FUNCTION_RESULT)                                    \
      return std::unexpected(ERR(error::Code::Unix));                                    \
    return SYSTEM_CALL_FUNCTION_RESULT;                                                  \
  }

/** Auto generate wrapper function for system calls function  */
#define GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(                                       \
  OK_TYPE, FAILURE_VALUE, WRAPPER_FUNCTION_SIGNATURE, SYSTEM_CALL_FUNCTION_NAME, ...)    \
  std::expected<void, error::Err> WRAPPER_FUNCTION_SIGNATURE noexcept                    \
  {                                                                                      \
    OK_TYPE SYSTEM_CALL_FUNCTION_RESULT = SYSTEM_CALL_FUNCTION_NAME(__VA_ARGS__);        \
    if (FAILURE_VALUE == SYSTEM_CALL_FUNCTION_RESULT)                                    \
      return std::unexpected(ERR(error::Code::Unix));                                    \
    return {};                                                                           \
  }

namespace bonding::unix
{
  class Filesystem
  {
  public:
    static std::expected<void, error::Err> Mkdir(const std::string & path) noexcept;
    static std::expected<std::string, error::Err>
      read_entire_file(const std::string & path) noexcept;
    static std::expected<int, error::Err>
      Open(const std::string & file, int flag) noexcept;
    static std::expected<int, error::Err>
      Open(const std::string & file, int flag, int mode) noexcept;
    static std::expected<void, error::Err> Rmdir(const std::string & file) noexcept;
    static std::expected<void, error::Err> Close(int fd) noexcept;
    static std::expected<void, error::Err> Write(int fd, const std::string & s) noexcept;
    static std::expected<void, error::Err>
      Write(const std::string & path, const std::string & s) noexcept;
  };

  class Capabilities
  {
  public:
    static std::expected<cap_t, error::Err> Get_proc() noexcept;
    static std::expected<void, error::Err>  Set_proc(cap_t cap) noexcept;
    static std::expected<void, error::Err>  Free(cap_t cap) noexcept;

    static std::expected<void, error::Err> Set_flag(
      cap_t               cap_p,
      cap_flag_t          flag,
      int                 ncap,
      const cap_value_t * caps,
      cap_flag_value_t    value) noexcept;
  };

  class Utsname
  {
  public:
    static std::expected<utsname, error::Err> Get() noexcept;
  };
}; // namespace bonding::unix

#endif /* BONDING_UNIX_H */