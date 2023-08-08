#ifndef __BONDING_RESOURCE_H__
#define __BONDING_RESOURCE_H__

#include "error.h"
#include "result.hpp"
#include <cstdint>

namespace bonding::resource
{
  class Cgroups
  {
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

    static Result<Void, error::Err>
    setup(const std::string hostname,
          const std::vector<Cgroups::Control> & cgroups) noexcept;

   public:
    inline static const std::string memory = "1073741824";
    inline static const std::string cpu_shares = "256";
    inline static const std::string pids = "64";
    inline static const std::string weight = "10";
    inline static const std::uint32_t fd_count = 64;
  };

  class Resource
  {
   public:
    static Result<Void, error::Err> setup(const std::string hostname) noexcept;

   private:
    inline static const int64_t KMEM_LIMIT = 1024 * 1024 * 1024;
    inline static const int64_t MEM_LIMIT = KMEM_LIMIT;
    inline static const int64_t MAX_PID = 64;
    inline static const uint64_t NOFILE_RLIMIT = 64;

   private:
    inline static const struct Cgroups::Control::Setting add_to_tasks = { .name = "tasks",
                                                                          .value = "0" };

    inline static const struct std::vector<Cgroups::Control> cgroups =
    {
      (Cgroups::Control{
	  .control = "memory",
	  .settings = { (Cgroups::Control::Setting{ .name = "memory.limit_in_bytes",
						    .value = Cgroups::memory }),
			add_to_tasks } }),

        (Cgroups::Control { .control = "cpu",
			    .settings =
			    {
			      (Cgroups::Control::Setting { 
                                .name = "cpu.shares", .value = Cgroups::cpu_shares }),
			      add_to_tasks,
			    } }),

        (Cgroups::Control { .control = "pids",
			    .settings =
			    {
			      (Cgroups::Control::Setting { .name = "pids.max",
							   .value = Cgroups::pids }),
			      add_to_tasks} }),
        (Cgroups::Control { .control = "blkio",
			    .settings = {
			      ( Cgroups::Control::Setting {
				.name = "blkio.bfq.weight", .value = Cgroups::pids }),
			      add_to_tasks,} }),
    };
  };
};

#endif /* __BONDING_RESOURCE_H__ */
