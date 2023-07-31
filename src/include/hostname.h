#ifndef __BONDING_HOSTNAME_H__
#define __BONDING_HOSTNAME_H__

#include "error.h"
#include "result.hpp"

namespace bonding::hostname
{
  /** Use the Xorshift algorithm to generate a random string
   ** with a length between 0 and 255, consisting of uppercase
   ** and lowercase letters and numbers from 0 to 9 */
  class Xorshift
  {
   public:
    static std::string generate(const uint8_t len) noexcept;

   private:
    /** Xorshift core algorithm */
    static uint32_t __xorshift() noexcept;

   private:
    inline static uint32_t X = 123456789;
    inline static uint32_t Y = 362436069;
    inline static uint32_t Z = 521288629;
    inline static uint32_t W = 88675123;
  };

  /** A hostname is what identifies our machine compared
   ** to every other living on the same network.
   ** It is used by many different networking softwares. */
  class Hostname
  {
   public:
    static Result<std::string, error::Err> generate(const uint8_t len) noexcept;

   private:
    inline static const uint8_t MAX_LENGTH = 10;
  };
}

#endif /* __BONDING_HOSTNAME_H__ */