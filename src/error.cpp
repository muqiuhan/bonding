#include "include/error.h"

namespace bonding::error
{

  void
  exit_with_return_code(Result<Unit, const Err> result) noexcept
  {
    if (result.is_ok())
      {
        spdlog::debug("Exit without any error, returning 0");
        exit(0);
      }
    else
      {
        const Err err = result.unwrap_err();
        const int32_t return_code = err.to_exit_code();

        spdlog::debug("Error on exit:\n\t{}\n\tReturning: {}", err.to_string(), return_code);

        exit(return_code);
      }
  }

  int32_t
  Err::to_exit_code() const noexcept
  {
    return 1;
  }

  std::string
  Err::to_string() const noexcept
  {
    return "";
  }

} // namespace bonding::error