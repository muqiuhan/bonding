/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#include "include/hostname.h"
#include "spdlog/spdlog.h"

namespace bonding::hostname
{
  uint32_t
  Xorshift::__xorshift() noexcept
  {
    uint32_t t = X ^ (X << 11);
    X = Y;
    Y = Z;
    Z = W;
    W = W ^ (W >> 19) ^ (t ^ (t >> 8));
    return W;
  }

  std::string
  Xorshift::generate(const uint8_t len) noexcept
  {
    std::vector<uint8_t> random_str((sizeof(char) * len) + 1);

    for (uint8_t index = 0; index < len; index++)
      {
        const uint32_t r = __xorshift();

        if (isdigit(r % 62))
          random_str[index] = (uint8_t)(r % 10 + 48);
        else if (isalpha(r % 62))
          random_str[index] = (uint8_t)(r % 26 + 65);
        else
          random_str[index] = (uint8_t)(r % 26 + 97);

        X = Y;
        Y = Z;
        Z = W;
        W = W ^ (W >> 19) ^ (r ^ (r >> 8));
      }

    return std::string(random_str.begin(), random_str.end());
  }

  Result<std::string, error::Err>
  Hostname::generate(const uint8_t len) noexcept
  {
    if (len > MAX_LENGTH)
      return Err(
        error::Err(error::Code::HostnameError,
                   "The maximum length of Hostname is: " + std::to_string(MAX_LENGTH)));

    return Ok(Xorshift::generate(MAX_LENGTH));
  }

  Result<Void, error::Err>
  Hostname::setup(const std::string hostname) noexcept
  {
    if (-1 == sethostname(hostname.c_str(), hostname.size()))
      {
        spdlog::error("Cannot set hostname {} for container", hostname);
        return ERR(error::Code::HostnameError);
      }

    spdlog::info("Container hostname is now {}", hostname);
    return Ok(Void());
  }
}
