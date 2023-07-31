#ifndef __BONDING_CLI_H__
#define __BONDING_CLI_H__

#include "error.h"
#include "result.hpp"
#include <structopt/app.hpp>
#include <sys/socket.h>

namespace bonding::cli
{

  struct Args
  {
    /** Activate debug mode */
    bool debug = false;

    /** Command to execute inside the container */
    std::string command;

    /** User ID to create inside the container */
    std::uint32_t uid;

    /** Directory to mount as root of the container */
    std::string mount_dir;
  };

  struct Command_Line_Args
  {
    std::optional<bool> debug = false;
    std::optional<std::string> command;
    std::optional<std::uint32_t> uid;
    std::optional<std::string> mount_dir;

   public:
    /** This function verifies that the command line arguments are valid
     ** This function is not modified by noexcept in order to throw the
     ** corresponding exception when the corresponding command parameter
     ** value cannot be obtained, and print the help information after being caught */
    Args to_args();
  };

  /** Creating a pair of sockets to give one to the child process,
   ** and one to the parent process. */
  Result<std::pair<int, int>, error::Err> generate_socketpair() noexcept;
};

/** Command line argument parser generated by structopt */
STRUCTOPT(bonding::cli::Command_Line_Args, debug, command, uid, mount_dir);

#endif /* __BONDING_CLI_H__ */