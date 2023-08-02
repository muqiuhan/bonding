#include "include/cli.h"
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

    if (!hostname.has_value())
      spdlog::warn("If hostname is not provided, it will be generated automatically");

    return Args{ debug.value(),
                 command.value(),
                 uid.value(),
                 mount_dir.value(),
                 hostname.value() };
  }

  Result<std::pair<int, int>, error::Err>
  generate_socketpair() noexcept
  {
    int __fds[2] = { 0 };
    /* creating a Unix domain socket, and socket will use a communication semantic with
     * packets and fixed length datagrams.*/
    if (-1 == socketpair(AF_UNIX, SOCK_SEQPACKET, 0, __fds))
      return Err(bonding::error::Err(error::Code::SocketError));

    return Ok(std::make_pair(__fds[0], __fds[1]));
  }
}
