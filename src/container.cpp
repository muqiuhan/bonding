/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/container.h"
#include "include/config.h"
#include "include/namespace.h"
#include "include/resource.h"
#include "include/syscall.h"

namespace bonding::container
{
  Result<Void, error::Err>
  Container::create() noexcept
  {
    ns::Namespace::handle_child_uid_map(m_child_process.m_pid, m_sockets.first);
    resource::Resource::setup(m_config.hostname);
    return m_child_process.wait();
  }

  Result<Void, error::Err>
  Container::clean_and_exit() noexcept
  {
    Container_Cleaner::close_socket(m_sockets.first).unwrap();
    Container_Cleaner::close_socket(m_sockets.second).unwrap();
    resource::Resource::clean(m_config.hostname);
    syscall::Syscall::Syscall::clean().unwrap();
    return Ok(Void());
  }

  Result<Void, error::Err>
  Container::start(const config::Container_Options& argv) noexcept
  {
    Container container(argv);

    if (argv.debug)
      {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Activate debug mode!");
      }

    return container.create()
      .and_then([&](const auto _) {
        spdlog::info("Cleaning and exiting container...");
        container.clean_and_exit().unwrap();
        return Ok(Void());
      })
      .or_else([&](const error::Err e) {
        container.clean_and_exit().unwrap();
        spdlog::error("Error while creating container: {}", e.to_string());
        return Err(e);
      });
  }

  Result<Void, error::Err>
  Container_Cleaner::close_socket(const int socket) noexcept
  {
    spdlog::debug("Closing socket {}...", socket);
    if (-1 == close(socket))
      {
        spdlog::error("Unable to close socket: {}", socket);
        return ERR(error::Code::Socket);
      }
    return Ok(Void());
  }
}
