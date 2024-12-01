#include "similarity.hpp"

#include <cstdint>
#include <span>
#include <unordered_map>

template <typename T>
    requires std::forward_iterator<T>
std::unordered_map<std::int64_t, std::int64_t> get_freqs(T begin, T end)
{
    std::unordered_map<std::int64_t, std::int64_t> result;
    for (auto iter { begin }; iter != end; ++iter) {
        ++result[*iter];
    }
    return result;
}

std::int64_t similarity_score(std::span<const std::int64_t> v1, std::span<const std::int64_t> v2)
{
    const auto freqs = get_freqs(v2.begin(), v2.end());
    std::int64_t score { 0 };

    for (auto x : v1) {
        auto it = freqs.find(x);
        if (it != freqs.end()) {
            score += x * it->second;
        }
    }

    return score;
}
