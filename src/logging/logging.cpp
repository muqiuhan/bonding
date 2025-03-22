#include "public/logging.h"

namespace bonding::logging
{
  /// Sets the logging level for the application.
  /// This initializes the logger with a ColorConsoleAppender that formats log messages
  /// with colors based on their severity.
  /// @param _level The desired logging level to set.
  auto set_level(const LOG_LEVEL & _level) noexcept -> void
  {
    static appender::ColorConsoleAppender<formatter::Formatter> appender;
    init(_level, &appender);
  }
} // namespace bonding::logging
