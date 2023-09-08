/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_LOG_HPP
#define BONDING_LOG_HPP

#include <plog/Formatters/TxtFormatter.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Log.h>

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

  static inline void
  set_level(const LOG_LEVEL & level) noexcept
  {
    plog::init<plog::TxtFormatter>(level, plog::streamStdOut);
  }
};

#endif /* BONDING_LOG_HPP */