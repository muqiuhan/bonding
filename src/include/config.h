/** Copyright (C) 2023 Muqiu Han <muqiu-han@outlook.com> */

#ifndef BONDING_CONFIG_H
#define BONDING_CONFIG_H

#include "cli.h"
#include "error.h"
#include "result.hpp"

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace bonding::config
{

  /** Extract the command line arguments into this class
   ** and initialize a Container struct that will have to perform
   ** the container work. */
  class Container_Options
  {
   public:
    /** Active debug mode */
    bool debug;

    /** The path of the binary executable script to execute inside the container. */
    std::string path;

    /** The path of the directory to use as a root inside container. */
    std::string mount_dir;

    /** The ID of the user inside the container.
     ** An ID of 0 means it’s root (administrator). */
    uint32_t uid;

    /** The full arguments passed (including the path option) into the commandline. */
    std::vector<std::string> argv;

    /** socket for IPC */
    std::pair<int, int> ipc;

    /** identifies machine */
    std::string hostname;

    /** Additional mount path */
    std::vector<std::pair<std::string, std::string>> mounts;

    /** The child process clone flags mask */
    int clone_flags;

    /** Cgroups-v1 control options */
    std::vector<std::pair<std::string, std::string>> cgroups_options;
  };

};

#endif /* BONDING_CONFIG_H */
