#ifndef __BONDING_CHILD_H__
#define __BONDING_CHILD_H__

#include "config.h"
#include "error.h"
#include "result.hpp"
#include <spdlog/spdlog.h>
#include <unistd.h>

namespace bonding::child
{

  /** The container child process cloned from parent process.
   ** That is, the process that runs the command provided by
   ** the `cli::Args.m_command` */
  class Child
  {
   public:
    explicit Child(const bonding::config::Container_Options container_options)
      : m_container_options(container_options)
      , m_pid(generate_child_process(container_options).unwrap())
    {
      spdlog::info("Starting container with command `{}` on process {}",
                   container_options.m_path,
                   m_pid);
    }

    Child()
      : m_container_options(bonding::config::Container_Options())
      , m_pid(-1)
    {
      std::terminate();
    }

    /* Wait for the child to finish. */
    Result<Unit, error::Err> wait() const noexcept;

   private:
    class Process
    {
     public:
      /** Make a copy of bonding::config::Container_Optionsm, instead of using
       ** the reference of the parent process */
      static int __main(void * options) noexcept;

     private:
      inline static bonding::config::Container_Options container_options;
    };

   private:
    static Result<pid_t, error::Err> generate_child_process(
      const bonding::config::Container_Options container_options) noexcept;

   private:
    inline static const uint32_t STACK_SIZE = 1024 * 1024;

    const bonding::config::Container_Options m_container_options;

   public:
    const pid_t m_pid;
  };
}

#endif /* __BONDING_CHILD_H__ */