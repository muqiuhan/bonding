/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CHECK_H
#define BONDING_CHECK_H

#include <expected>
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
    static std::expected<std::pair<int, int>, error::Err>
      parse_version(const utsname & host);

  public:
    Kernel()
      : host(unix::Utsname::Get().value())
      , version(parse_version(host).value())
      , domain_name(host.domainname)
      , machine(host.machine)
      , release(host.release)
      , sysname(host.sysname)
      , node_name(host.nodename)

    {
      LOG_DEBUG << "Get host machine information...";
      LOG_DEBUG << "System: " << sysname << ", Machine: " << machine
                << ", Release: " << release << ", Node: " << node_name
                << ", Domain: " << domain_name;
    }

  public:
    std::pair<int, int> version;
    const std::string   domain_name;
    const std::string   machine;
    const std::string   release;
    const std::string   sysname;
    const std::string   node_name;
  };

  class CgroupsV1
  {
  private:
    inline static const std::string PATH = "/sys/fs/cgroup/";

  public:
    static std::expected<bool, error::Err>
      checking_if_controller_supported(const std::string & controller) noexcept;
  };

  class Info
  {
  public:
    inline static const Kernel kernel;
  };
}; // namespace bonding::environment

#endif /* BONDING_CHECK_H */