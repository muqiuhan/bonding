/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/namespace.h"
#include "include/ipc.h"
#include <fcntl.h>
#include <grp.h>
#include <sched.h>
#include <unistd.h>

namespace bonding::ns
{
  std::expected<bool, error::Err> Namespace::has_user_namespace() noexcept
  {
    /* Unshare the user namespace, so that the calling process is
     * moved into a new user namespace which is not shared with
     * any previously existing process. */
    if (-1 == unshare(CLONE_NEWUSER))
      return false;

    return true;
  }

  std::expected<void, error::Err>
    Namespace::setup(const int socket, const uid_t uid) noexcept
  {
    LOG_DEBUG << "Setting up user namespace with UID " << uid;

    const bool  has_userns = has_user_namespace().value();
    const gid_t gid = uid;

    ipc::IPC::send_boolean(socket, has_userns).value();
    if (ipc::IPC::recv_boolean(socket))
      return std::unexpected(ERR(error::Code::Namespace));

    if (has_userns)
      LOG_INFO << "Setting user namespace...✓";
    else
      LOG_ERROR << "Setting user namespace...✗";

    /* set the list of groups the process is part of */
    if (-1 == setgroups(1, groups))
      return std::unexpected(ERR(error::Code::Namespace));

    /* set the UID and GID (respectively) of the process.
     * this will set the real user ID, the effective user ID,
     * and the saved set-user-ID. */
    if (-1 == setresgid(gid, gid, gid))
      return std::unexpected(ERR(error::Code::Namespace));

    if (-1 == setresuid(uid, uid, uid))
      return std::unexpected(ERR(error::Code::Namespace));

    return {};
  }

  std::expected<void, error::Err>
    Namespace::create_map(const int id, const std::string map) noexcept
  {
    const std::string path = "/proc/" + std::to_string(id) + "/" + map;
    const std::string data =
      "0 " + std::to_string(USERNS_OFFSET) + " " + std::to_string(USERNS_COUNT);

    const int fd =
      creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    if (-1 == fd)
      return std::unexpected(ERR(error::Code::Namespace));

    if (-1 == write(fd, data.c_str(), data.size()))
      return std::unexpected(ERR(error::Code::Namespace));

    return {};
  }

  std::expected<void, error::Err> Namespace::handle_child_uid_map(const pid_t pid) noexcept
  {

    create_map(pid, "uid_map").value();
    create_map(pid, "gid_map").value();

    return {};
  }

} // namespace bonding::ns
