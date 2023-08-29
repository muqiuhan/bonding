/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CHECK_H
#define BONDING_CHECK_H

#include "result.hpp"
#include "unix.h"
#include <map>
#include <sys/utsname.h>
#include <vector>

namespace bonding::environment
{
  class Kernel
  {
   private:
    const utsname host;

   private:
    static Result<std::pair<int, int>, error::Err> parse_version(const utsname & host);

   public:
    Kernel()
      : host(unix::Utsname::get().unwrap())
      , version(parse_version(host).unwrap())
      , machine(host.machine)
      , release(host.release)
      , sysname(host.sysname)
      , node_name(host.nodename)
      , domain_name(host.domainname)
    {
      spdlog::debug("Get host machine information...");
      spdlog::debug("System: {}, Machine: {}, Release: {}, Node: {}, Domain: {}",
                    sysname,
                    machine,
                    release,
                    node_name,
                    domain_name);
    }

   public:
    std::pair<int, int> version;
    const std::string domain_name;
    const std::string machine;
    const std::string release;
    const std::string sysname;
    const std::string node_name;
  };

  class CgroupsV1
  {
   private:
    inline static const std::string PATH = "/sys/fs/cgroup/";

   public:
    static Result<bool, error::Err>
    check_support_controller(const std::string & controller) noexcept;
  };

  class Info
  {
   public:
    inline static const Kernel kernel;
  };
};

#endif /* BONDING_CHECK_H */