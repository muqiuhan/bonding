#include "private/formatter.h"
#include "iomanip"

namespace bonding::logging::formatter
{
  using namespace plog;

  [[nodiscard]] auto Formatter::header() noexcept -> util::nstring { return {}; }

  [[nodiscard]] auto Formatter::format(const plog::Record & record) noexcept
    -> util::nstring
  {
    tm t{};
    util::localtime_s(&t, &record.getTime().time);
    util::nostringstream ss;

    ss << "| " << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2)
       << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2)
       << t.tm_mday << PLOG_NSTR(" ") << std::setfill(PLOG_NSTR('0')) << std::setw(2)
       << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2)
       << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2)
       << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3)
       << static_cast<int>(record.getTime().millitm) << PLOG_NSTR(" > ")
       << record.getMessage() << PLOG_NSTR("\n");

    return ss.str();
  }
} // namespace bonding::logging::formatter