/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CAPABILITIES_H
#define BONDING_CAPABILITIES_H

#include "error.h"
#include "result.hpp"
#include "unix.h"
#include <vector>

namespace bonding::capabilities
{

  /** Linux capabilities are a way to divide the almighty administrator
   ** role with supreme powers over the machine into a set of limited
   ** powers controlling isolated systems and processes of the system.  */
  class Capabilities
  {
  public:
    static Result<Void, error::Err> setup() noexcept;

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