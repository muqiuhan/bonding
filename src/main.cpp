#include "spdlog/spdlog.h"
#include "structopt/app.hpp"

#include <string>

struct Options
{
  /* Activate debug mode */
  std::optional<bool> debug = false;

  /* Command to execute inside the container */
  std::string command;

  /* User ID to create inside the container */
  std::uint32_t uid;

  /* Directory to mount as root of the container */
  std::string mount_dir;
};
STRUCTOPT (Options, debug, command, uid, mount_dir);

int
main (int argc, char **argv)
{
  try
    {
      auto options = structopt::app ("bonding").parse<Options> (argc, argv);

      if (options.debug.value())
          spdlog::info ("Activate debug mode!");

      std::cout << "command: " << options.command << std::endl;
      std::cout << "uid: " << options.uid << std::endl;
      std::cout << "mount_dir: " << options.mount_dir << std::endl;
    }
  catch (structopt::exception &e)
    {
      std::cout << e.what () << "\n";
      std::cout << e.help ();
    }
  return 0;
}
