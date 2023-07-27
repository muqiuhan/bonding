#include "include/cli.h"
#include "structopt/app.hpp"
#include <spdlog/spdlog.h>

namespace bonding::cli
{
  Args
  Command_Line_Args::to_args()
  {
    if (!command.has_value())
      spdlog::error("The `command` parameter must be provided!");

    if (!mount_dir.has_value())
      spdlog::error("The `mount-dir` parameter must be provided!");

    if (!uid.has_value())
      spdlog::error("The `uid` parameter must be provided!");

    return Args{ debug.value(), command.value(), uid.value(), mount_dir.value() };
  }

  Result<std::pair<int, int>, const error::Code>
  generate_socketpair() noexcept
  {
    int sockets[2] = { 0 };
    /* creating a Unix domain socket, and socket will use a communication semantic with
     * packets and fixed length datagrams.*/
    if (-1 == socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets))
      return Err(error::Err(Code::SocketError));

    return Ok(std::make_pair(sockets[0], sockets[1]);
  }
}
