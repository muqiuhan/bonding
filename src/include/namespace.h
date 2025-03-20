/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_NAMESPACE_H
#define BONDING_NAMESPACE_H

#include "error.h"
#include <expected>
#include <sched.h>
#include <sys/types.h>

namespace bonding::ns
{
  /** User namespaces allows process to virtually be root inside their
   ** own namespace, but not on the parent system.
   ** It also allows the child namespace to have its own users / groups configuration. */
  class Namespace
  {
  public:
    /** Executed by the child process during its configuration. */
    static std::expected<void, error::Err> setup(int socket, uid_t uid) noexcept;

    /** Called by the container when it will perform UID / GID mapping. */
    static std::expected<void, error::Err> handle_child_uid_map(pid_t pid) noexcept;

  private:
    /** If that call is successful, then user namespaces are supported. */
    static std::expected<bool, error::Err> has_user_namespace() noexcept;

    static std::expected<void, error::Err> create_map(int id, std::string map) noexcept;

  private:
    inline static gid_t groups[1];

    /** mapped by the system to a real UID >10000,
     ** and it can manage its users and groups
     ** without polluting the parent system. */
    static inline const uint64_t USERNS_OFFSET = 10000;
    static inline const uint64_t USERNS_COUNT = 2000;
  };
}; // namespace bonding::ns

#endif /* BONDING_NAMESPACE_H */
