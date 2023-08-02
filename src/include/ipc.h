#ifndef __BONDING_IPC_H__
#define __BONDING_IPC_H__

#include "error.h"
#include "result.hpp"

namespace bonding::ipc
{

  /** A wrapper to ease use of the sockets to IPC */
  class IPC
  {
   public:
    static Result<Unit, error::Err> send_boolean(const int socket,
                                                 const bool data) noexcept;
    static Result<bool, error::Err> recv_boolean(const int socket) noexcept;
  };
}

#endif /* __BONDING_IPC_H__ */
