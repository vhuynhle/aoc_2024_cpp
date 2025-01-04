#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <print>
#include <utility>
#include <vector>

using Cache = std::map<std::pair<std::uint64_t, std::uint64_t>, std::uint64_t>;

std::optional<std::pair<std::uint64_t, std::uint64_t>> split(std::uint64_t number) {
    std::uint64_t num_digits{0};
    for (std::uint64_t n{number}; n != 0; n /= 10) {
        ++num_digits;
    }

    if ((num_digits & 1) == 1) {
        return {};
    }

    std::uint64_t first_half{number};
    std::uint64_t denominator{1};
    for (std::uint64_t i{0}; i < (num_digits / 2); ++i) {
        first_half /= 10;
        denominator *= 10;
    }

    const std::uint64_t second_half = number - first_half * denominator;
    return std::make_pair(first_half, second_half);
}

std::uint64_t blink(std::uint64_t x, std::uint64_t n, Cache& cache) {
    if (n == 0) {
        return 1;
    }

    const auto key = std::make_pair(x, n);
    auto& value = cache[key];
    if (value == 0) {  // not yet calculated
        if (x == 0) {
            value = blink(1, n - 1, cache);
        } else {
            auto t = split(x);
            if (t.has_value()) {
                const auto [first, second] = t.value();
                value = blink(first, n - 1, cache) + blink(second, n - 1, cache);
            } else {
                value = blink(x * 2024, n - 1, cache);
            }
        }
    }
    return value;
}

int main() {
    constexpr std::uint64_t p1_rounds{25};
    constexpr std::uint64_t p2_rounds{75};

    std::vector<std::uint64_t> numbers;
    std::uint64_t num;
    while (std::cin >> num) {
        numbers.push_back(num);
    }

    std::map<std::pair<std::uint64_t, std::uint64_t>, std::uint64_t> cache;

    std::uint64_t p1_res{0};
    for (auto num : numbers) {
        p1_res += blink(num, p1_rounds, cache);
    }
    std::println("Part 1 result: {}", p1_res);

    std::uint64_t p2_res{0};
    for (auto num : numbers) {
        p2_res += blink(num, p2_rounds, cache);
    }
    std::println("Part 2 result: {}", p2_res);

    return 0;
}
