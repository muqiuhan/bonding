/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_UNIX_H
#define BONDING_UNIX_H

#include "error.h"
#include "result.hpp"
#include <linux/prctl.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/utsname.h>

/** Auto generate wrapper function for system calls function  */
#define GENERATE_SYSTEM_CALL_WRAPPER(OK_TYPE,                                            \
                                     FAILURE_VALUE,                                      \
                                     WRAPPER_FUNCTION_SIGNATURE,                         \
                                     SYSTEM_CALL_FUNCTION_NAME,                          \
                                     ...)                                                \
  Result<OK_TYPE, error::Err> WRAPPER_FUNCTION_SIGNATURE noexcept                        \
  {                                                                                      \
    OK_TYPE SYSTEM_CALL_FUNCTION_RESULT = SYSTEM_CALL_FUNCTION_NAME(__VA_ARGS__);        \
    if (FAILURE_VALUE == SYSTEM_CALL_FUNCTION_RESULT)                                    \
      return ERR(error::Code::Unix);                                                     \
    return Ok(SYSTEM_CALL_FUNCTION_RESULT);                                              \
  }

/** Auto generate wrapper function for system calls function  */
#define GENERATE_NO_RET_VALUE_SYSTEM_CALL_WRAPPER(OK_TYPE,                               \
                                                  FAILURE_VALUE,                         \
                                                  WRAPPER_FUNCTION_SIGNATURE,            \
                                                  SYSTEM_CALL_FUNCTION_NAME,             \
                                                  ...)                                   \
  Result<Void, error::Err> WRAPPER_FUNCTION_SIGNATURE noexcept                           \
  {                                                                                      \
    OK_TYPE SYSTEM_CALL_FUNCTION_RESULT = SYSTEM_CALL_FUNCTION_NAME(__VA_ARGS__);        \
    if (FAILURE_VALUE == SYSTEM_CALL_FUNCTION_RESULT)                                    \
      return ERR(error::Code::Unix);                                                     \
    return Ok(Void());                                                                   \
  }

namespace bonding::unix
{
  class Filesystem
  {
   public:
    static Result<Void, error::Err> Mkdir(const std::string & path) noexcept;
    static Result<std::string, error::Err>
    read_entire_file(const std::string & path) noexcept;
    static Result<int, error::Err> Open(const std::string & file, int flag) noexcept;
    static Result<Void, error::Err> Rmdir(const std::string & file) noexcept;
    static Result<Void, error::Err> Close(int fd) noexcept;
    static Result<Void, error::Err> Write(int fd, const std::string & s) noexcept;
  };

  class Capabilities
  {
   public:
    static Result<cap_t, error::Err> Get_proc() noexcept;
    static Result<Void, error::Err> Set_proc(cap_t cap) noexcept;
    static Result<Void, error::Err> Free(cap_t cap) noexcept;

    static Result<Void, error::Err> Set_flag(cap_t cap_p,
                                             cap_flag_t flag,
                                             int ncap,
                                             const cap_value_t * caps,
                                             cap_flag_value_t value) noexcept;
  };

  class Utsname
  {
   public:
    static Result<utsname, error::Err> Get() noexcept;
  };
};

#endif /* BONDING_UNIX_H */