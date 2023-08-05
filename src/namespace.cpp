#include "include/namespace.h"
#include "include/ipc.h"
#include <fcntl.h>
#include <grp.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace bonding::ns
{
  Result<bool, error::Err>
  Namespace::has_user_namespace() noexcept
  {
    /* Unshare the user namespace, so that the calling process is
     * moved into a new user namespace which is not shared with
     * any previously existing process. */
    if (-1 == unshare(CLONE_NEWUSER))
      return Ok(false);

    return Ok(true);
  }

  Result<Unit, error::Err>
  Namespace::setup(const int socket, const uid_t uid) noexcept
  {
    spdlog::debug("Setting up user namespace with UID {}", uid);
    const bool has_userns = has_user_namespace().unwrap();
    const gid_t gid = uid;

    ipc::IPC::send_boolean(socket, has_userns).unwrap();
    if (ipc::IPC::recv_boolean(socket).unwrap())
      return Err(bonding::error::Err(bonding::error::Code::NamespaceError));

    if (has_userns)
      spdlog::info("User namespace setup...");
    else
      spdlog::warn("User namespace not supported, continuting...");

    /* set the list of groups the process is part of */
    if (-1 == setgroups(1, groups))
      return Err(bonding::error::Err(bonding::error::Code::NamespaceError));

    /* set the UID and GID (respectively) of the process.
     * this will set the real user ID, the effective user ID,
     * and the saved set-user-ID. */
    if (-1 == setresgid(gid, gid, gid))
      return Err(bonding::error::Err(bonding::error::Code::NamespaceError));

    if (-1 == setresuid(uid, uid, uid))
      return Err(bonding::error::Err(bonding::error::Code::NamespaceError));

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Namespace::create_map(const int id, const std::string map) noexcept
  {
    const std::string path = "/proc/" + std::to_string(id) + "/" + map;
    const int fd =
      creat(path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == fd)
      return Err(error::Err(error::Code::NamespaceError));

    const std::string data =
      "0 " + std::to_string(USERNS_OFFSET) + " " + std::to_string(USERNS_COUNT);

    if (-1 == write(fd, data.c_str(), data.size()))
      return Err(error::Err(error::Code::NamespaceError));

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Namespace::handle_child_uid_map(const pid_t pid, const int socket) noexcept
  {
    if (ipc::IPC::recv_boolean(socket).unwrap())
      {
        create_map(pid, "uid_map").unwrap();
        create_map(pid, "gid_map").unwrap();
      }
    else
      {
        spdlog::warn("No user namespace set up from child process");
        return Ok(Unit());
      }

    spdlog::debug("Child UID/GID map done, sending signal to child to continue...");
    ipc::IPC::send_boolean(socket, false);

    return Ok(Unit());
  }

}
