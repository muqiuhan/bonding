/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_IPC_H
#define BONDING_IPC_H

#include "error.h"
#include <expected>

namespace bonding::ipc
{

  /** A wrapper to ease use of the sockets to IPC */
  class IPC
  {
  public:
    static std::expected<void, error::Err> send_boolean(int socket, bool data) noexcept;
    static std::expected<bool, error::Err> recv_boolean(int socket) noexcept;
  };
} // namespace bonding::ipc

#endif /* BONDING_IPC_H */
