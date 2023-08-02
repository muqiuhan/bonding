#ifndef __BONDING_NAMESPACE_H__
#define __BONDING_NAMESPACE_H__

#include "error.h"
#include "result.hpp"
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
    Namespace(const int socket, const uid_t uid)
      : m_socket(socket)
      , m_uid(uid)
      , m_gid(uid)
      , m_groups(uid)
    {
    }

    /** Executed by the child process during its configuration. */
    Result<Unit, error::Err> setup() const noexcept;

    /** Called by the container when it will perform UID / GID mapping. */
    static Result<Unit, error::Err> handle_child_uid_map(const pid_t pid,
                                                         const int socket) noexcept;

   private:
    /** If that call is successful, then user namespaces are supported. */
    Result<bool, error::Err> has_user_namespace() const noexcept;
    static Result<Unit, error::Err> create_map(const int id,
                                               const std::string map) noexcept;

   private:
    const int m_socket;
    const uid_t m_uid;
    const gid_t m_gid;
    const gid_t m_groups[1];

    /** mapped by the system to a real UID >10000,
     ** and it can manage its users and groups
     ** without polluting the parent system. */
    static inline const uint64_t USERNS_OFFSET = 10000;
    static inline const uint64_t USERNS_COUNT = 2000;
  };
};

#endif /* __BONDING_NAMESPACE_H__ */