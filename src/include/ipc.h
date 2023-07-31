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
    Result<Unit, error::Err> send_boolean(bool data) const noexcept;
    Result<bool, error::Err> recv_boolean(bool data) const noexcept;

   private:
    const int m_socket;
  };
}

#endif /* __BONDING_IPC_H__ */