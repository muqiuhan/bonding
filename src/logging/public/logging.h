#pragma once

#include "../private/appender.h"

namespace bonding::logging
{
  /// The help function to set the logging level for the application.
  /// This function should be called to configure the logging behavior.
  /// @param _level The desired logging level to set.
  auto set_level(const LOG_LEVEL & _level) noexcept -> void;
} // namespace bonding::logging