#include <gtest/gtest.h>
#include "log.hpp"
#include <fc/log/logger.hpp>

using namespace std;

int main(int argc, char **argv) {
    auto seed = time(NULL);
    std::srand(seed);
    std::cout << "Random number generator seeded to " << seed << std::endl;

    bool is_verbose = false;
    std::string verbose_arg = "--verbose";
    for (int i = 0; i < argc; i++) {
       if (verbose_arg == argv[i]) {
          is_verbose = true;
          break;
       }
    }

    if (!is_verbose) {
        std::cout << "Logging disabled, use --verbose to enable" << std::endl;
        logger.enabled(false);
        fc::logger::get().set_log_level(fc::log_level::off);
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
