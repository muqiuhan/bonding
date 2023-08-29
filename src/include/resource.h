/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_RESOURCE_H
#define BONDING_RESOURCE_H

#include "error.h"
#include "result.hpp"
#include <cstdint>

namespace bonding::resource
{
  /** Cgroups is a mechanism introduced in Linux v2.6.4 which allows to “allocate”
   ** ressources for a group of processes. As this feature has been reworked, two versions
   ** coexist in the Linux kernel. The main difference for us is that the v2 groups all
   ** the configuration for a given group under the same directory. */
  class CgroupsV1
  {
   private:
    inline static const std::string MEM_LIMIT = std::to_string(1024 * 1024 * 1024);
    inline static const std::string CPU_SHARES = "256";
    inline static const std::string BLKIO_BFQ_WEIGHT = "10";
    inline static const std::string PIDS_MAX = "64";

   public:
    struct Control
    {
      const std::string control;

      struct Setting
      {
        const std::string name;
        const std::string value;
      };

      const std::vector<Setting> settings;
    };

    static Result<Void, error::Err> setup(std::string hostname) noexcept;

    /** After the child process exited, container need to clean
     ** all the cgroups restriction added.
     ** NOTE: This is very simple as cgroups v2 centralises everything in a directory
     **       under /sys/fs/cgroup/<groupname>/ */
    static Result<Void, error::Err> clean(const std::string & hostname) noexcept;

   private:
    static Result<Void, error::Err>
    write_settings(const std::string & dir,
                   const CgroupsV1::Control::Setting & setting) noexcept;

    static Result<Void, error::Err>
    write_contorl(const std::string hostname, const CgroupsV1::Control & cgroup) noexcept;

    static Result<Void, error::Err>
    clean_control_task(const CgroupsV1::Control & control) noexcept;

   private:
    inline static const struct Control::Setting TASK = { .name = "tasks", .value = "0" };
    inline static const struct std::vector<Control> CONFIG =
      std::vector<CgroupsV1::Control>
    {
      (Control{ .control = "memory",
                .settings = { (Control::Setting{ .name = "memory.limit_in_bytes",
                                                 .value = MEM_LIMIT }) } }),

        (Control{ .control = "cpu",
                  .settings = { (Control::Setting{ .name = "cpu.shares",
                                                   .value = CPU_SHARES }) } }),

        (Control{
          .control = "pids",
          .settings = { (Control::Setting{ .name = "pids.max", .value = PIDS_MAX }) } }),
        (Control{ .control = "blkio",
                  .settings = { (Control::Setting{ .name = "blkio.bfq.weight",
                                                   .value = PIDS_MAX }) } })
    };
  };

  /** Rlimit is a system used to restrict a single process.
   ** It’s focus is more centered around what this process can do than what realtime
   ** system ressources it consumes. */
  class Rlimit
  {
   public:
    static Result<Void, error::Err> setup() noexcept;

   private:
    /** The file descriptor number, like the number of pids, is per-user, and  prevent
     ** in-container process from occupying all of them. */
    inline static const uint64_t NOFILE = 64;
  };

  class Resource
  {
   public:
    static Result<Void, error::Err> setup(const std::string & hostname) noexcept;
    static Result<Void, error::Err> clean(const std::string & hostname) noexcept;
  };
};

#endif /* BONDING_RESOURCE_H */
