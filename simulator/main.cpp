#include <gtest/gtest.h>

using namespace std;

constexpr uint32_t random_seed = 42;

int main(int argc, char **argv) {
    auto seed = time(NULL);
    std::srand(seed);
    std::cout << "Random number generator seeded to " << time(NULL) << std::endl;
    std::cout << seed;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
