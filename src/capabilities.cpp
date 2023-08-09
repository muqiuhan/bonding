/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/capabilities.h"
#include "spdlog/spdlog.h"
#include <error.h>
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
        return Err(error::Err(error::Code::CapabilitiesError));

    cap_t caps = NULL;
    if (!(caps = cap_get_proc())
        || cap_set_flag(caps, CAP_INHERITABLE, DROP.size(), &DROP[0], CAP_CLEAR)
        || cap_set_proc(caps))
      {
        if (NULL != caps)
          cap_free(caps);

        return Err(error::Err(error::Code::CapabilitiesError));
      }

    cap_free(caps);
    return Ok(Void());
  }
}