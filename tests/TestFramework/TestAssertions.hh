#ifndef TEST_ASSERTIONS_HH
#define TEST_ASSERTIONS_HH

#include <iostream>

#define TEST_ASSERT(cond)                         \
    do {                                          \
        if (!(cond)) {                            \
            std::cerr << "Assertion failed: "     \
                      << #cond                    \
                      << " (" << __FILE__         \
                      << ":" << __LINE__ << ")"   \
                      << std::endl;               \
            return false;                         \
        }                                         \
    } while (0)

#endif
