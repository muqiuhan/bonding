/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CHILD_H
#define BONDING_CHILD_H

#include "config.h"
#include "error.h"
#include "log.hpp"
#include "result.hpp"
#include <unistd.h>

namespace bonding::child
{

  /** The container child process cloned from parent process.
   ** That is, the process that runs the command provided by
   ** the `cli::Args.m_command` */
  class Child
  {
   public:
    explicit Child(const config::Container_Options & container_options)
      : m_container_options(container_options)
      , m_pid(generate_child_process(container_options).unwrap())
    {
      LOG_INFO << "Starting container with command " << container_options.path
               << " on process " << m_pid;
    }

    Child()
      : m_container_options(config::Container_Options())
      , m_pid(-1)
    {
      std::terminate();
    }

    /* Wait for the child to finish. */
    [[nodiscard]] Result<Void, error::Err> wait() const noexcept;

   private:
    class Process
    {
     private:
      inline static config::Container_Options * container_options;

     public:
      inline static const uint32_t STACK_SIZE = 1024 * 1024;
      inline static void * STACK = malloc(STACK_SIZE);

     public:
      /** Make a copy of config::Container_Optionsm, instead of using
       ** the reference of the parent process */
      [[maybe_unused]] static int _main(void * options) noexcept;

      static Result<Void, error::Err> setup_container_configurations() noexcept;
    };

   private:
    static Result<pid_t, error::Err>
    generate_child_process(config::Container_Options container_options) noexcept;

   private:
    const config::Container_Options m_container_options;

   public:
    const pid_t m_pid;
  };
}

#endif /* BONDING_CHILD_H */
