#include "include/container.h"
#include "include/namespace.h"

namespace bonding::container
{
  Result<Void, error::Err>
  Container::create() noexcept
  {
    spdlog::info("Container is READY!!!");
    ns::Namespace::handle_child_uid_map(m_child_process.m_pid, m_sockets.first);
    m_child_process.wait();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Container::clean_and_exit() noexcept
  {
    Container_Cleaner::close_socket(m_sockets.first).unwrap();
    Container_Cleaner::close_socket(m_sockets.second).unwrap();

    return Ok(Void());
  }

  Result<Void, error::Err>
  Container::start(const cli::Args argv) noexcept
  {
    Container container = Container::make(argv).unwrap();

    return container.create()
      .and_then([&](const auto _) {
        spdlog::info("Cleaning and exiting container...");
        container.clean_and_exit();
        return Ok(Void());
      })
      .or_else([&](const error::Err e) {
        container.clean_and_exit();
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
        return Err(error::Err(bonding::error::Code::SocketError));
      }
    return Ok(Void());
  }
}
