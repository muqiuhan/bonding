#include "include/container.h"

namespace bonding::container
{
  Result<Unit, error::Err>
  Container::create() noexcept
  {
    spdlog::debug("Creation finished.");
    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Container::clean_and_exit() noexcept
  {
    spdlog::debug("Cleaning container");
    return Ok(Unit());
  }

  Result<Unit, error::Err>
  Container::start(const cli::Args argv) noexcept
  {
    Container container(argv);

    return container.create()
      .and_then([&](const auto _) {
        spdlog::debug("Finished, clean and exit");
        container.clean_and_exit();
        return Ok(Unit());
      })
      .or_else([&](const error::Err e) {
        container.clean_and_exit();
        spdlog::error("Error while creating container: {}", e.to_string());
        return Err(e);
      });
  }
}
