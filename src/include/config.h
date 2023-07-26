#ifndef __BONDING_CONFIG_H__
#define __BONDING_CONFIG_H__

#include "result.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace bonding::config
{

  /** Extract the command line arguments into this class
   ** and initialize a Container struct that will have to perform
   ** the container work. */
  class Container_Options
  {
   public:
    Container_Options(const std::string command, const std::string mount_dir, const uint32_t uid)
      : m_mount_dir(mount_dir)
      , m_uid(uid)
      , m_argv(parse_argv(command).expect("Cannot parse command arguments"))
      , m_path(m_argv.at(0))
    {
    }

   private:
    static Result<std::vector<std::string>, Unit> parse_argv(const std::string & argv) noexcept;

   private:
    /* The path of the binary / executable / script to execute inside the container. */
    const std::string m_path;

    /* The path of the directory we want to use as a / root inside our container. */
    const std::string m_mount_dir;

    /* The ID of the user inside the container. An ID of 0 means it’s root (administrator). */
    const uint32_t m_uid;

    /* The full arguments passed (including the path option) into the commandline. */
    const std::vector<std::string> m_argv;
  };

};

#endif /* __BONDING_CONFIG_HPP__ */