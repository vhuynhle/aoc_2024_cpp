#include "distance.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

std::int64_t distance(std::vector<std::int64_t>&& v1, std::vector<std::int64_t>&& v2)
{
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("distance: Two input vectors must have the same length");
    }

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    std::int64_t result { 0 };
    for (std::size_t i { 0 }; i < v1.size(); ++i) {
        result += std::abs(v1[i] - v2[i]);
    }

    return result;
}
