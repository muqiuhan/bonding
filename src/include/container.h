#ifndef __BONDING_CONTAINER_H__
#define __BONDING_CONTAINER_H__

#include "cli.h"
#include "config.h"
#include "error.h"

namespace bonding::container
{
  class Container
  {
   public:
    static Result<Container, error::Err>
    make(const cli::Args & args) noexcept
    {
      /* creates the ContainerOpts struct from the commandline arguments. */
      return Ok(
        Container(config::Container_Options::make(args.command, args.mount_dir, args.uid)
                    .unwrap()));
    }

    Container()
      : m_config(bonding::config::Container_Options())
      , m_sockets(std::make_pair(-1, -1))
    {
      std::terminate();
    }

   private:
    explicit Container(
      std::pair<config::Container_Options, std::pair<int, int>> config_and_sockets)
      : Container(config_and_sockets.first, config_and_sockets.second)
    {
    }

    Container(config::Container_Options config, std::pair<int, int> sockets)
      : m_config(config)
      , m_sockets(sockets)
    {
    }

   public:
    /** handle the container creation process. */
    Result<Unit, error::Err> create() noexcept;

    /** called before each exit to be sure we stay clean. */
    Result<Unit, error::Err> clean_and_exit() noexcept;

    /** get the args from the commandline and handle everything
     ** from the struct Container creation to the exit.
     ** returns a Result that will inform if an error happened during the process. */
    static Result<Unit, error::Err> start(const cli::Args argv) noexcept;

   private:
    const config::Container_Options m_config;
    const std::pair<int, int> m_sockets;
  };

  class Container_Cleaner
  {
   public:
    static Result<Unit, error::Err> close_socket(const int socket) noexcept;
  };
};

#endif /* __BONDING_CONTAINER_H__ */