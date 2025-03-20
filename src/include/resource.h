/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_RESOURCE_H
#define BONDING_RESOURCE_H

#include "config.h"
#include "error.h"
#include <expected>

#include <cstdint>

namespace bonding::resource
{
  /** Cgroups is a mechanism introduced in Linux v2.6.4 which allows to “allocate”
   ** ressources for a group of processes. As this feature has been reworked, two versions
   ** coexist in the Linux kernel. The main difference for us is that the v2 groups all
   ** the configuration for a given group under the same directory. */
  class CgroupsV1
  {
  public:
    static std::expected<void, error::Err>
      setup(const bonding::config::Container_Options & config) noexcept;

    /** After the child process exited, container need to clean
     ** all the cgroups restriction added.
     ** NOTE: This is very simple as cgroups v2 centralises everything in a directory
     **       under /sys/fs/cgroup/<groupname>/ */
    static std::expected<void, error::Err>
      clean(const config::Container_Options & config) noexcept;

  private:
    static std::expected<void, error::Err> write_settings(
      const std::string &                         dir,
      const config::CgroupsV1::Control::Setting & setting) noexcept;

    static std::expected<void, error::Err> write_contorl(
      const std::string hostname, const config::CgroupsV1::Control & cgroup) noexcept;

    static std::expected<void, error::Err>
      clean_control_task(const config::CgroupsV1::Control & control) noexcept;

  private:
    inline static const struct config::CgroupsV1::Control::Setting TASK = {
      .name = "tasks", .value = "0"};
  };

  /** Rlimit is a system used to restrict a single process.
   ** It’s focus is more centered around what this process can do than what realtime
   ** system ressources it consumes. */
  class Rlimit
  {
  public:
    static std::expected<void, error::Err> setup() noexcept;

  private:
    /** The file descriptor number, like the number of pids, is per-user, and  prevent
     ** in-container process from occupying all of them. */
    inline static const uint64_t NOFILE = 64;
  };

  class Resource
  {
  public:
    static std::expected<void, error::Err>
      setup(const config::Container_Options & config) noexcept;
    static std::expected<void, error::Err>
      clean(const config::Container_Options & config) noexcept;
  };
}; // namespace bonding::resource

#endif /* BONDING_RESOURCE_H */
