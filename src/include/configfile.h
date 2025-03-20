#ifndef BONDING_CONFIGFILE_H
#define BONDING_CONFIGFILE_H

#include "config.h"
#include "error.h"
#include <expected>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace bonding::configfile
{
  class Config_File
  {
  public:
    static std::expected<config::Container_Options, error::Err>
      read(const std::string & file) noexcept;

    static std::expected<std::string, error::Err>
      generate_default(std::string hostname, std::string command) noexcept;

  private:
    static std::expected<nlohmann::json, error::Err>
      parse(const std::string & str) noexcept;
    static std::expected<config::Container_Options, error::Err>
      Container_Options_of_json(const nlohmann::json & json) noexcept;

    static std::expected<std::vector<std::pair<std::string, std::string>>, error::Err>
      read_mounts(const nlohmann::json & data) noexcept;

    static std::expected<int, error::Err> read_clone(const nlohmann::json & data) noexcept;

    static std::expected<std::vector<config::CgroupsV1::Control>, error::Err>
      read_cgroups_options(const nlohmann::json & data) noexcept;

    static std::expected<std::vector<std::string>, error::Err>
      parse_argv(std::string argv) noexcept;

    static std::expected<std::pair<int, int>, error::Err> generate_socketpair() noexcept;
  };

  inline static const std::map<std::string, uint32_t> CLONE_FLAGS_MAP = {
    {"CLONE_CHILD_CLEARTID", CLONE_CHILD_CLEARTID},
    {"CLONE_CHILD_SETTID", CLONE_CHILD_SETTID},
    // {"CLONE_CLEAR_SIGHAND", CLONE_CLEAR_SIGHAND},
    {"CLONE_DETACHED", CLONE_DETACHED},
    {"CLONE_FILES", CLONE_FILES},
    {"CLONE_FS", CLONE_FS},
    // {"CLONE_INTO_CGROUP", CLONE_INTO_CGROUP},
    {"CLONE_IO", CLONE_IO},
    {"CLONE_NEWCGROUP", CLONE_NEWCGROUP},
    {"CLONE_NEWIPC", CLONE_NEWIPC},
    {"CLONE_NEWNET", CLONE_NEWNET},
    {"CLONE_NEWNS", CLONE_NEWNS},
    {"CLONE_NEWPID", CLONE_NEWPID},
    {"CLONE_NEWUSER", CLONE_NEWUSER},
    {"CLONE_NEWUTS", CLONE_NEWUTS},
    {"CLONE_PARENT", CLONE_PARENT},
    {"CLONE_PARENT_SETTID", CLONE_PARENT_SETTID},
    // {"CLONE_PID", CLONE_PID},
    {"CLONE_PIDFD", CLONE_PIDFD},
    {"CLONE_PTRACE", CLONE_PTRACE},
    {"CLONE_SETTLS", CLONE_SETTLS},
    {"CLONE_SIGHAND", CLONE_SIGHAND},
    // {"CLONE_STOPPED", CLONE_STOPPED},
    {"CLONE_SYSVSEM", CLONE_SYSVSEM},
    {"CLONE_THREAD", CLONE_THREAD},
    {"CLONE_UNTRACED", CLONE_UNTRACED},
    {"CLONE_VFORK", CLONE_VFORK},
    {"CLONE_VM", CLONE_VM},
  };
} // namespace bonding::configfile

#endif /* BONDING_CONFIGFILE_H */