/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/cli.h"
#include "include/hostname.h"
#include <spdlog/spdlog.h>
#include <vector>

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

    if (hostname.value() == "")
      spdlog::warn("If hostname is not provided, it will be generated automatically");

    if (!mounts.has_value())
      spdlog::warn("The `mounts` parameter is not provided, use the default {}",
                   "{\"/lib64:/lib64\", \"/lib:/lib\"}");

    return Args{ debug.value(),
                 command.value(),
                 uid.value(),
                 mount_dir.value(),
                 "bonding."
                   + (hostname.value() == "" ? hostname::Hostname::generate(10).unwrap()
                                             : hostname.value()),

                 (mounts.has_value() ? parse_add_path(mounts.value()).unwrap()
                                     : std::vector<std::pair<std::string, std::string>>{
                                       { "/lib64", "/lib64" },
                                       { "/lib", "/lib" } }) };
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

  Result<std::vector<std::pair<std::string, std::string>>, error::Err>
  Command_Line_Args::parse_add_path(
    const std::vector<std::string> & cli_add_paths) noexcept
  {
    std::vector<std::pair<std::string, std::string>> add_paths;

    for (const std::string & path : cli_add_paths)
      {
        const std::string::size_type index = path.find(':');

        if (index != std::string::npos)
          add_paths.push_back(
            std::make_pair(path.substr(0, index), path.substr(index + 1)));
      }

    return Ok(add_paths);
  }
}
