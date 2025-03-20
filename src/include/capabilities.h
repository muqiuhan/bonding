/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CAPABILITIES_H
#define BONDING_CAPABILITIES_H

#include "error.h"
#include "unix.h"
#include <vector>
#include <expected>

namespace bonding::capabilities
{

  /** Linux capabilities are a way to divide the almighty administrator
   ** role with supreme powers over the machine into a set of limited
   ** powers controlling isolated systems and processes of the system.  */
  class Capabilities
  {
  public:
    inline static std::expected<void, error::Err> setup() noexcept
    {
      for (const int drop_caps : DROP)
        if (-1 == prctl(PR_CAPBSET_DROP, drop_caps, 0, 0, 0))
          return std::unexpected(ERR(error::Code::Capabilities));

      unix::Capabilities::Get_proc()
        .transform([](cap_t cap) {
        unix::Capabilities::Set_flag(
          cap, CAP_INHERITABLE, static_cast<int>(DROP.size()), &DROP[0], CAP_CLEAR)
          .and_then([&]() { return unix::Capabilities::Set_proc(cap); })
          .and_then([&]() {
          return unix::Capabilities::Free(cap);
        }).transform_error([&](const error::Err & err) {
          if (nullptr != cap)
            unix::Capabilities::Free(cap).value();

          return ERR_MSG(error::Code::Capabilities, err.to_string());
        }).value();

        return cap;
      }).value();

      LOG_INFO << "Clearing unwanted capabilities...✓";

      return {};
    }

  private:
    /** The list of the capabilities we will restrict: */
    inline const static std::vector<cap_value_t> DROP = {
      CAP_AUDIT_READ,      CAP_AUDIT_READ,   CAP_AUDIT_WRITE,  CAP_BLOCK_SUSPEND,
      CAP_DAC_READ_SEARCH, CAP_DAC_OVERRIDE, CAP_FSETID,       CAP_IPC_LOCK,
      CAP_MAC_ADMIN,       CAP_MAC_OVERRIDE, CAP_MKNOD,        CAP_SETFCAP,
      CAP_SYSLOG,          CAP_SYS_ADMIN,    CAP_SYS_BOOT,     CAP_SYS_MODULE,
      CAP_SYS_NICE,        CAP_SYS_RAWIO,    CAP_SYS_RESOURCE, CAP_SYS_TIME,
      CAP_WAKE_ALARM};
  };
} // namespace bonding::capabilities

#endif /* BONDING_CAPABILITIES_H */