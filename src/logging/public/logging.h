#pragma once

#include "../private/appender.h"
#include "../private/formatter.h"
#include "plog/Log.h"

namespace bonding::logging
{
  /// The help function to set the logging level for the application.
  /// This function should be called to configure the logging behavior.
  /// @param __level The desired logging level to set.
  auto set_level(const LOG_LEVEL & __level) noexcept -> void;
} // namespace bonding::logging