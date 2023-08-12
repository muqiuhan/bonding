/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/unix.h"
#include <cstddef>
#include <error.h>
#include <filesystem>
#include <sys/capability.h>
#include <sys/utsname.h>

namespace bonding::unix
{
  Result<Void, error::Err>
  Filesystem::mkdir(const std::string & path) noexcept
  {
    try
      {
        std::filesystem::create_directories(path);
      }
    catch (...)
      {
        spdlog::error("Cannot create direcotry {}");
        return ERR(error::Code::MountsError);
      }

    return Ok(Void());
  }

  /** Capabilities::get_proc */
  GENERATE_SYSTEM_CALL_WRAPPER(cap_t, NULL, Capabilities::get_proc(), cap_get_proc)

  /** Capabilities::set_proc */
  GENERATE_SYSTEM_CALL_WRAPPER(
    int, -1, Capabilities::set_proc(cap_t cap), cap_set_proc, cap)

  /** Capabilities::free */
  GENERATE_SYSTEM_CALL_WRAPPER(int, -1, Capabilities::free(cap_t cap), cap_free, cap)

  /** Capabilities::set_flag */
  GENERATE_SYSTEM_CALL_WRAPPER(int,
                               -1,
                               Capabilities::set_flag(cap_t cap,
                                                      cap_flag_t flag,
                                                      int ncap,
                                                      const cap_value_t * caps,
                                                      cap_flag_value_t value),
                               cap_set_flag,
                               cap,
                               flag,
                               ncap,
                               caps,
                               value)

  Result<utsname, error::Err>
  Utsname::get() noexcept
  {
    struct utsname host = { 0 };
    if (-1 == uname(&host))
      return ERR(error::Code::UnixError);

    return Ok(host);
  }

}