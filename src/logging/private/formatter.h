#pragma once

#include <plog/Log.h>
#include <plog/Util.h>

namespace bonding::logging::formatter
{
  using namespace plog;

  /// Formatting log records into string representations.
  class Formatter
  {
  public:
    /// Returns the header for the log format.
    /// This can be used to provide a title or initial information for the log output.
    [[nodiscard]] static auto header() noexcept -> util::nstring;

    /// Formats a given log record into a string representation.
    /// This includes the timestamp, severity, and message of the log record.
    /// @param record The log record to format.
    /// @return A formatted string representation of the log record.
    [[nodiscard]] static auto format(const plog::Record & record) noexcept
      -> util::nstring;
  }; // namespace bonding::logging::formatter
} // namespace bonding::logging::formatter