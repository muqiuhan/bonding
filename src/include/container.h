/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CONTAINER_H
#define BONDING_CONTAINER_H

#include "child.h"
#include "cli.h"
#include "config.h"
#include "error.h"
#include <expected>

namespace bonding::container
{
  class Container
  {
  public:
    Container()
      : m_config(config::Container_Options())
      , m_sockets(std::make_pair(-1, -1))
      , m_child_process(child::Child())
    {
      std::terminate();
    }

  private:
    explicit Container(const config::Container_Options & config)
      : m_config(config), m_sockets(config.ipc), m_child_process(child::Child(config))
    {}

  public:
    /** handle the container creation process. */
    std::expected<void, error::Err> create() noexcept;

    /** called before each exit to be sure we stay clean. */
    std::expected<void, error::Err> clean_and_exit() noexcept;

    /** get the args from the commandline and handle everything
     ** from the struct Container creation to the exit.
     ** returns a Result that will inform if an error happened during the process. */
    static std::expected<void, error::Err> start(const config::Container_Options & argv) noexcept;

  private:
    const config::Container_Options m_config;
    const std::pair<int, int>       m_sockets;
    const child::Child              m_child_process;
  };

  class Container_Cleaner
  {
  public:
    static std::expected<void, error::Err> close_socket(const int socket) noexcept;
  };
}; // namespace bonding::container

#endif /* BONDING_CONTAINER_H */