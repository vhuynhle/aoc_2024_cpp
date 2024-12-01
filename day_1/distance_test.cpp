#include "distance.hpp"

#include <gtest/gtest.h>

TEST(Distance, SampleTest)
{
    ASSERT_EQ(distance((std::vector<std::int64_t> { 3, 4, 2, 1, 3, 3 }),
                  (std::vector<std::int64_t> { 4, 3, 5, 3, 9, 3 })),
        11);
}
