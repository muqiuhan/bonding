/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

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
    int uid;

    /** Directory to mount as root of the container */
    std::string mount_dir;

    /** Hostname to identifies container */
    std::string hostname;

    /** Additional mount path */
    std::vector<std::pair<std::string, std::string>> mounts;
  };

  struct Command_Line_Args
  {
    std::optional<bool> debug = false;
    std::optional<std::string> hostname = "";
    std::optional<std::string> config;
    std::optional<std::string> command = "";
    std::optional<int> uid = -1;
    std::optional<std::string> mount_dir = "";
    std::optional<std::vector<std::string>> mounts = {};

   public:
    /** This function verifies that the command line arguments are valid
     ** This function is not modified by noexcept in order to throw the
     ** corresponding exception when the corresponding command parameter
     ** value cannot be obtained, and print the help information after being caught */
    Args to_args();
    
    void check_args() const noexcept;
    std::string check_hostname() const noexcept;
    bool check_config_file() const noexcept;
    std::vector<std::pair<std::string, std::string>> check_mounts() const noexcept;

    static Result<std::vector<std::pair<std::string, std::string>>, error::Err>
    parse_add_path(const std::vector<std::string> & add_path) noexcept;
  };

  /** Creating a pair of sockets to give one to the child process,
   ** and one to the parent process. */
  Result<std::pair<int, int>, error::Err> generate_socketpair() noexcept;
};

/** Command line argument parser generated by structopt */
STRUCTOPT(bonding::cli::Command_Line_Args,
          debug,
          command,
          uid,
          mount_dir,
          hostname,
          mounts,
          config);

#endif /* __BONDING_CLI_H__ */
