#ifndef __BONDING_CONTAINER_H__
#define __BONDING_CONTAINER_H__

#include "cli.h"
#include "config.h"
#include "error.h"

namespace bonding::container
{
  class Container
  {
   public:
    explicit Container(const cli::Args & args)
      /* creates the ContainerOpts struct from the commandline arguments. */
      : m_config(config::Container_Options(args.command, args.mount_dir, args.uid))
    {
    }

   public:
    /** handle the container creation process. */
    Result<Unit, error::Err> create() noexcept;

    /** called before each exit to be sure we stay clean. */
    Result<Unit, error::Err> clean_and_exit() noexcept;

    /** get the args from the commandline and handle everything 
     ** from the struct Container creation to the exit.
     ** returns a Result that will inform if an error happened during the process. */
    static Result<Unit, error::Err> start(const cli::Args argv) noexcept;

   private:
    const config::Container_Options m_config;
  };
};

#endif /* __BONDING_CONTAINER_H__ */