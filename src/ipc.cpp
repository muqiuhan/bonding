#include "include/ipc.h"
#include <sys/socket.h>

namespace bonding::ipc
{
  Result<Unit, error::Err>
  IPC::send_boolean(bool data) const noexcept
  {
    const uint8_t buf[1] = { data };

    /* With a zero flags argument, send() is equivalent to write(). */
    if (-1 == send(m_socket, &buf, 1, 0))
      {
        spdlog::error("Cannot send boolean through socket: {}", m_socket);
        return Err(bonding::error::Err(bonding::error::Code::SocketError));
      }
    return Ok(Unit());
  }

  Result<bool, error::Err>
  IPC::recv_boolean(bool data) const noexcept
  {
    uint8_t buf[1] = { 0 };

    /* With a zero flags argument, recv() is generally equivalent to read() */
    if (-1 == recv(m_socket, &buf, 1, 0))
      {
        spdlog::error("Cannot recevie boolean from socket {}", m_socket);
        return Err(bonding::error::Err(bonding::error::Code::SocketError));
      }

    return Ok(static_cast<bool>(buf[0]));
  }
}