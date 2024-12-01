#include "distance.hpp"

#include <gtest/gtest.h>

TEST(Distance, SampleTest)
{
    auto v1 = std::vector<std::int64_t> { 3, 4, 2, 1, 3, 3 };
    auto v2 = std::vector<std::int64_t> { 4, 3, 5, 3, 9, 3 };
    ASSERT_EQ(distance(v1, v2), 11);
}
