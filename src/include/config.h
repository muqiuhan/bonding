/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef __BONDING_CONFIG_H__
#define __BONDING_CONFIG_H__

#include "cli.h"
#include "error.h"
#include "result.hpp"

#include <cstdint>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace bonding::config
{

  /** Extract the command line arguments into this class
   ** and initialize a Container struct that will have to perform
   ** the container work. */
  class Container_Options
  {
   public:
    inline static Result<std::pair<Container_Options, std::pair<int, int>>, error::Err>
    make(const cli::Args & args) noexcept
    {
      const std::pair<int, int> sockets = cli::generate_socketpair().unwrap();
      const std::vector<std::string> argv =
        parse_argv(args.command).expect("Cannot parse command arguments");

      return Ok(std::make_pair(Container_Options(argv,
                                                 args.debug,
                                                 args.mount_dir,
                                                 args.uid,
                                                 sockets.second,
                                                 args.hostname,
                                                 args.random_hostname,
                                                 args.mounts),
                               sockets));
    }

    inline const Container_Options
    operator=(const Container_Options & container_options) const noexcept
    {
      return container_options;
    }

    Container_Options()
      : m_mount_dir("")
      , m_uid(-1)
      , m_argv({})
      , m_path("")
      , m_raw_fd(0)
      , m_hostname("")
      , m_random_hostname(false)
      , m_debug(false)
      , m_mounts(
          std::vector<std::pair<std::string, std::string>>{ std::make_pair("", "") })
    {
      std::terminate();
    }

   private:
    Container_Options(const std::vector<std::string> & argv,
                      const bool debug,
                      const std::string mount_dir,
                      const uint32_t uid,
                      const int raw_fd,
                      const std::string hostname,
                      const bool random_hostname,
                      const std::vector<std::pair<std::string, std::string>> mounts)
      : m_argv(argv)
      , m_path(argv.at(0))
      , m_mount_dir(std::move(mount_dir))
      , m_uid(uid)
      , m_raw_fd(raw_fd)
      , m_hostname(std::move(hostname))
      , m_mounts(mounts)
      , m_random_hostname(random_hostname)
      , m_debug(debug)
    {
    }

    static Result<std::vector<std::string>, Void>
    parse_argv(const std::string argv) noexcept;

   public:
    const bool m_debug;
    /** The path of the binary executable script to execute inside the container. */
    const std::string m_path;

    /** The path of the directory to use as a root inside container. */
    const std::string m_mount_dir;

    /** The ID of the user inside the container.
     ** An ID of 0 means it’s root (administrator). */
    const uint32_t m_uid;

    /** The full arguments passed (including the path option) into the commandline. */
    const std::vector<std::string> m_argv;

    /** socket for IPC */
    const int m_raw_fd;

    /** identifies machine */
    const std::string m_hostname;
    const bool m_random_hostname;

    /** Additional mount path */
    const std::vector<std::pair<std::string, std::string>> m_mounts;
  };

};

#endif /* __BONDING_CONFIG_HPP__ */
