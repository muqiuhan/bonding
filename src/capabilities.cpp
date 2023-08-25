/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/capabilities.h"
#include "include/unix.h"
#include "spdlog/spdlog.h"
#include <linux/prctl.h>
#include <sys/prctl.h>

namespace bonding::capabilities
{
  Result<Void, error::Err>
  Capabilities::setup() noexcept
  {
    spdlog::debug("Clearing unwanted capabilities...");

    for (const int drop_caps : DROP)
      if (-1 == prctl(PR_CAPBSET_DROP, drop_caps, 0, 0, 0))
        return ERR(error::Code::Capabilities);

    unix::Capabilities::get_proc()
      .and_then([](cap_t cap) {
        unix::Capabilities::set_flag(cap,
                                     CAP_INHERITABLE,
                                     static_cast<int>(DROP.size()),
                                     &DROP[0],
                                     CAP_CLEAR)
          .and_then([&](const int _) { return unix::Capabilities::set_proc(cap); })
          .and_then([&](const int _) { return unix::Capabilities::free(cap); })
          .or_else([&](const error::Err & err) {
            if (nullptr != cap)
              unix::Capabilities::free(cap).unwrap();

            return ERR_MSG(error::Code::Capabilities, err.to_string());
          });

        return Ok(cap);
      })
      .unwrap();

    return Ok(Void());
  }
}