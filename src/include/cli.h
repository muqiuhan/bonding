#ifndef __BONDING_CLI_H__
#define __BONDING_CLI_H__

#define _DEFAULT_SOURCE
#include <stdio.h>

#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/** Command-line arguments to specify the behavior of the container */
struct cli_args
{
  /** Activate debug mode */
  bool debug;

  /** Command to execute inside the container */
  bool help;

  /** Directory to mount as root of the container */
  char *mount_dir;

  /** Command to execute inside the container */
  char *command;

  /** User ID to create inside the container */
  uid_t uid;
};

/** Parsing from main's command line arguments to cli_args */
struct cli_args cli_args_parse (int argc, char *const *argv);

void print_usage ();

#endif /* __BONDING_CLI_H__ */