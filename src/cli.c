#include "include/cli.h"
#include "include/log.h"

void
print_usage ()
{
  puts ("Usage: bonding"
        "-m <mount_dir>"
        "-c <command>"
        "-u <uid>"
        "[-d] turn on debug mode"
        "[-h] show this message");
}

/** Initially parse the command line arguments in main to cli_args,
 ** and need to check later.
 ** Among them, help and debug are optional and have no arguments.
 ** And mount_dir, uid, command are all mandatory. */
struct cli_args
__cli_args_parse (int argc, char *const *argv)
{
  struct cli_args args = { false, false, NULL, NULL, 0 };

  int opt;
  while ((opt = getopt (argc, argv, "m:c:u:dh")) != -1)
    {
      switch (opt)
        {
        case 'm':
          args.mount_dir = optarg;
          break;
        case 'c':
          args.command = optarg;
          break;
        case 'u':
          args.uid = atoi (optarg);
          break;
        case 'd':
          args.debug = true;
          break;
        case 'h':
          args.help = true;
          break;
        default:
          print_usage ();
          exit (EXIT_FAILURE);
        }
    }

  return args;
}

/** Check that all parameters in cli_args are legal.
 ** If true == help, print the help information directly here and exit. */
struct cli_args
__cli_args_check (const struct cli_args args)
{
  if (args.mount_dir == NULL || args.command == NULL)
    {
      print_usage ();
      exit (EXIT_FAILURE);
    }

  if (args.help)
    {
      print_usage ();
      exit (EXIT_SUCCESS);
    }

  if (args.debug)
    {
      simplog.setLogDebugLevel (SIMPLOG_DEBUG);
      LOG_INFO ("Debug mode activated\n");
    }
  else
    {
      simplog.setLogDebugLevel (SIMPLOG_INFO);
    }

  return args;
}

/** Parsing from main's command line arguments to cli_args */
struct cli_args
cli_args_parse (int argc, char *const *argv)
{
  return __cli_args_check (__cli_args_parse (argc, argv));
}