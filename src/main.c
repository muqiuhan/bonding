#define _BSD_SOURCE

#include "include/cli.h"
#include "include/log.h"

int
main (int argc, char *const *argv)
{
  if (1 == argc)
    {
      log.writeLog (LOG_ERROR,
                    "You did not provide any command line arguments!");
      
            
      exit (EXIT_FAILURE);
    }

  struct cli_args args = cli_args_parse (argc, argv);

  printf ("Mount directory: %s\n", args.mount_dir);
  printf ("Command: %s\n", args.command);
  printf ("UID: %d\n", args.uid);

  return 0;
}
