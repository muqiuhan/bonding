#ifndef __BONDING_ERROR_H__
#define __BONDING_ERROR_H__

#include "result.hpp"
#include <spdlog/spdlog.h>
#include <string>

namespace bonding::error
{

  class Err
  {
   public:
    enum class Code
    {

    };

   public:
    Err(const Code & code)
      : m_code(code)
    {
    }

    int32_t to_exit_code() const noexcept;
    std::string to_string() const noexcept;

   private:
    const Code & m_code;
  };

  /** Get the result from a function, and exit the process with the correct error
   ** code. Linux executable returns a number when they exit, which describe how everything went
   ** a return code of 0 means that there was no errors, any other number describe an error and what that error is
   ** (based on the return code value). */
  void exit_with_return_code(Result<void, const Err &> result) noexcept;

}

#endif /* __BONDING_ERROR_H__ */