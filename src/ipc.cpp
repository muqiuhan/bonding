/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/ipc.h"
#include <sys/socket.h>

namespace bonding::ipc
{
  Result<Void, error::Err> IPC::send_boolean(const int socket, const bool data) noexcept
  {
    const uint8_t buf[1] = {data};

    /* With a zero flags argument, send() is equivalent to write(). */
    if (-1 == send(socket, &buf, 1, 0))
      return ERR_MSG(
        error::Code::Socket,
        "Cannot send boolean through socket: " + std::to_string(socket));
    return Ok(Void());
  }

  Result<bool, error::Err> IPC::recv_boolean(const int socket) noexcept
  {
    uint8_t buf[1] = {0};

    /* With a zero flags argument, recv() is generally equivalent to read() */
    if (-1 == recv(socket, &buf, 1, 0))
      return ERR_MSG(
        error::Code::Socket,
        "Cannot recevie boolean from socket" + std::to_string(socket));

    return Ok(static_cast<bool>(buf[0]));
  }
} // namespace bonding::ipc