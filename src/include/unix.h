/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef __BONDING_UNIX_H__
#define __BONDING_UNIX_H__

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
    const OK_TYPE SYSTEM_CALL_FUNCTION_RESULT = SYSTEM_CALL_FUNCTION_NAME(__VA_ARGS__);  \
    if (FAILURE_VALUE == SYSTEM_CALL_FUNCTION_RESULT)                                    \
      return ERR(error::Code::UnixError);                                                \
    return Ok(SYSTEM_CALL_FUNCTION_RESULT);                                              \
  }

namespace bonding::unix
{
  class Filesystem
  {
   public:
    static Result<Void, error::Err> mkdir(const std::string & path) noexcept;
    static Result<std::string, error::Err> read_entire_file(const std::string & path) noexcept;
  };

  class Capabilities
  {
   public:
    static Result<cap_t, error::Err> get_proc() noexcept;
    static Result<int, error::Err> set_proc(cap_t cap) noexcept;
    static Result<int, error::Err> free(cap_t cap) noexcept;

    static Result<int, error::Err> set_flag(cap_t cap_p,
                                            cap_flag_t flag,
                                            int ncap,
                                            const cap_value_t * caps,
                                            cap_flag_value_t value) noexcept;
  };

  class Utsname
  {
   public:
    static Result<utsname, error::Err> get() noexcept;
  };
};

#endif /* __BONDING_UNIX_H__ */