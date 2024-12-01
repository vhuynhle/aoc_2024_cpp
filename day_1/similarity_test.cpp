#include "similarity.hpp"

#include <gtest/gtest.h>

TEST(Similarity, SampleTest)
{
    const auto v1 = std::vector<std::int64_t> { 3, 4, 2, 1, 3, 3 };
    const auto v2 = std::vector<std::int64_t> { 4, 3, 5, 3, 9, 3 };

    ASSERT_EQ(similarity_score(v1, v2), 31);
}
