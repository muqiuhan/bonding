/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_LOG_HPP
#define BONDING_LOG_HPP

#include <iomanip>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Log.h>
#include <plog/Util.h>

#define LOG_INFO PLOG_INFO
#define LOG_DEBUG PLOG_DEBUG
#define LOG_ERROR PLOG_ERROR
#define LOG_WARNING PLOG_WARNING
#define LOG_FATAL PLOG_FATAL
#define LOG_LEVEL plog::Severity
#define LOG_LEVEL_INFO plog::Severity::info
#define LOG_LEVEL_DEBUG plog::Severity::debug
#define LOG_LEVEL_WARNING plog::Severity::warning
#define LOG_LEVEL_ERROR plog::Severity::error
#define LOG_LEVEL_FATAL plog::Severity::fatal

namespace bonding::log
{
  namespace formatter
  {
    using namespace plog;
    class Formatter
    {
     public:
      inline static util::nstring
      header() noexcept
      {
        return util::nstring();
      }

      inline static util::nstring
      format(const plog::Record & record) noexcept
      {
        tm t;
        util::localtime_s(&t, &record.getTime().time);
        util::nostringstream ss;

        ss << "| " << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0'))
           << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".")
           << std::setfill(PLOG_NSTR('0')) << std::setw(3)
           << static_cast<int>(record.getTime().millitm) << PLOG_NSTR(" > ")
           << record.getMessage() << PLOG_NSTR("\n");

        return ss.str();
      }
    };
  }

  namespace appender
  {
    using namespace plog;

    template <class Formatter>
    class PLOG_LINKAGE_HIDDEN ColorConsoleAppender : public ConsoleAppender<Formatter>
    {
     public:
      ColorConsoleAppender(OutputStream outStream = streamStdOut)
        : ConsoleAppender<Formatter>(outStream)
      {
      }

      virtual void
      write(const Record & record) noexcept PLOG_OVERRIDE
      {
        util::nstring str = Formatter::format(record);
        util::MutexLock lock(this->m_mutex);

        this->writestr(getColor(record.getSeverity()) + str + "\033[0m");
      }

     protected:
      std::string
      getColor(Severity severity) noexcept
      {
        if (this->m_isatty)
          {
            switch (severity)
              {
              case fatal:
                return "\x1B[97m\x1B[41m"; // white on red background

              case error:
                return "\x1B[91m"; // red

              case warning:
                return "\x1B[93m"; // yellow

              case debug:
                return "\x1B[96m"; // cyan

              case info:
                return "\x1B[92m"; // green

              default:
                return "";
              }
          }

        return "";
      }
    };
  }

  static inline void
  set_level(const LOG_LEVEL & __level) noexcept
  {
    static appender::ColorConsoleAppender<formatter::Formatter> appender;
    plog::init(__level, &appender);
  }
};

#endif /* BONDING_LOG_HPP */