#include "spdlog/spdlog.h"
#include "structopt/app.hpp"

#include "include/cli.h"
#include <string>

using namespace bonding;

int
main(int argc, char ** argv) {
    try {
        auto options = structopt::app("bonding").parse<cli::Args>(argc, argv);

        if (options.debug.value())
            spdlog::info("Activate debug mode!");

        std::cout << "command: " << options.command << std::endl;
        std::cout << "uid: " << options.uid << std::endl;
        std::cout << "mount_dir: " << options.mount_dir << std::endl;
    } catch (structopt::exception & e) {
        std::cout << e.what() << "\n";
        std::cout << e.help();
    }
    return 0;
}
