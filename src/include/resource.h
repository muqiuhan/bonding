#ifndef __BONDING_RESOURCE_H__
#define __BONDING_RESOURCE_H__

#include "error.h"
#include "result.hpp"
#include <cstdint>

namespace bonding::resource
{
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

    static Result<Void, error::Err> setup(const std::string hostname) noexcept;
    static Result<Void, error::Err> clean(const std::string hostname) noexcept;

   private:
    inline static const struct Control::Setting TASK = { .name = "tasks", .value = "0" };

    inline static const struct std::vector<Control> CONFIG =
    {
      (Control{
	  .control = "memory",
	  .settings = { (Control::Setting{ .name = "memory.limit_in_bytes",
						    .value = MEM_LIMIT }),
			TASK } }),

        (Control { .control = "cpu",
			    .settings =
			    {
			      (Control::Setting { 
                                .name = "cpu.shares", .value = CPU_SHARES }),
			      TASK,
			    } }),

        (Control { .control = "pids",
			    .settings =
			    {
			      (Control::Setting { .name = "pids.max",
							   .value = PIDS_MAX }),
			    TASK} }),
        (Control { .control = "blkio",
			    .settings = {
			      ( Control::Setting {
				.name = "blkio.bfq.weight", .value = PIDS_MAX }),
			      TASK} }),
    };
  };

  class Rlimit
  {
   public:
    static Result<Void, error::Err> setup() noexcept;

   private:
    inline static const uint64_t NOFILE = 64;
  };

  class Resource
  {
   public:
    static Result<Void, error::Err> setup(const std::string hostname) noexcept;
    static Result<Void, error::Err> clean(const std::string hostname) noexcept;
  };
};

#endif /* __BONDING_RESOURCE_H__ */
