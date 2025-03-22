#pragma once

#include "formatter.h"
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Log.h>
#include <plog/Util.h>

// Log levels using plog's Severity enumeration for better readability.
#define LOG_LEVEL         plog::Severity
#define LOG_LEVEL_INFO    plog::Severity::info
#define LOG_LEVEL_DEBUG   plog::Severity::debug
#define LOG_LEVEL_WARNING plog::Severity::warning
#define LOG_LEVEL_ERROR   plog::Severity::error
#define LOG_LEVEL_FATAL   plog::Severity::fatal

namespace bonding::logging::appender
{
  using namespace plog;

  /// Extends the ConsoleAppender to provide colored output based on the severity of the log messages.
  template <class Formatter>
  class PLOG_LINKAGE_HIDDEN ColorConsoleAppender : public ConsoleAppender<Formatter>
  {
  public:
    /// Initializes the ColorConsoleAppender with a specified output stream.
    /// @param outStream The output stream to which log messages will be written.
    ColorConsoleAppender(OutputStream outStream = streamStdOut)
      : ConsoleAppender<Formatter>(outStream)
    {}

    /// Writes a log record to the console with appropriate color coding based on
    /// severity.
    /// @param record The log record to write.
    virtual auto write(const Record & record) noexcept -> void PLOG_OVERRIDE;

  protected:
    /// Returns the color code for the specified severity level.
    /// @param severity The severity level of the log record.
    /// @return A string containing the ANSI color code for the specified severity.
    [[nodiscard]] auto get_color(Severity severity) noexcept -> std::string;
  };
}; // namespace bonding::logging::appender

namespace bonding::logging::appender
{
  using namespace plog;
  using namespace logging::formatter;

  template <typename T>
  auto ColorConsoleAppender<T>::write(const Record & record) noexcept -> void
  {
    util::nstring   str = Formatter::format(record);
    util::MutexLock lock(this->m_mutex);

    // Write the formatted string with the appropriate color.
    this->writestr(this->get_color(record.getSeverity()) + str + "\033[0m");
  }

  template <typename T>
  [[nodiscard]] auto ColorConsoleAppender<T>::get_color(Severity severity) noexcept
    -> std::string
  {
    if (this->m_isatty)
      {
        switch (severity)
          {
          case fatal: // White text on red background for fatal errors.
            return "\x1B[97m\x1B[41m";

          case error: // Red text for errors.
            return "\x1B[91m";

          case warning: // Yellow text for warnings.
            return "\x1B[93m";

          case debug: // Cyan text for debug messages.
            return "\x1B[96m";

          case info: // Green text for informational messages.
            return "\x1B[92m";

          default:
            return ""; // No color for other severities.
          }
      }

    return ""; // No color if not a terminal output.
  }
} // namespace bonding::logging::appender