#include "include/container.h"

namespace bonding::container
{
  Result<Unit, error::Err>
  Container::create() noexcept
  {
    spdlog::info("Container is READY!!!");
    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Container::clean_and_exit() noexcept
  {
    Container_Cleaner::close_socket(m_sockets.first).unwrap();
    Container_Cleaner::close_socket(m_sockets.second).unwrap();

    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Container::start(const cli::Args argv) noexcept
  {
    Container container = Container::make(argv).unwrap();

    return container.create()
      .and_then([&](const auto _) {
        spdlog::info("Cleaning and exiting container...");
        container.clean_and_exit();
        return Ok(Unit());
      })
      .or_else([&](const error::Err e) {
        container.clean_and_exit();
        spdlog::error("Error while creating container: {}", e.to_string());
        return Err(e);
      });
  }

  Result<Unit, error::Err>
  Container_Cleaner::close_socket(const int socket) noexcept
  {
    spdlog::debug("Closing socket {}...", socket);
    if (-1 == close(socket))
      {
        spdlog::error("Unable to close socket: {}", socket);
        return Err(error::Err(bonding::error::Code::SocketError));
      }
    return Ok(Unit());
  }
}
