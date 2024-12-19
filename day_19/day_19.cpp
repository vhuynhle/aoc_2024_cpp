#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <vector>

std::uint64_t count_ways_to_construct(const std::string& str,
    const std::vector<std::string>& patterns, std::map<std::string, std::uint64_t>& cache);

int main()
{
    auto line = std::string {};
    std::getline(std::cin, line);

    std::vector<std::string> patterns;
    std::string::size_type start { 0 };
    while (true) {
        const auto pos = line.find(',', start);
        if (pos == std::string::npos) {
            patterns.emplace_back(line.substr(start));
            break;
        }

        patterns.emplace_back(line.substr(start, pos - start));
        start = pos + 2;
    }

    std::getline(std::cin, line); // ignore the blank line

    std::map<std::string, std::uint64_t> cache;
    std::size_t constructable_count { 0 };
    std::size_t part2_res { 0 };
    while (std::getline(std::cin, line)) {
        const auto nways = count_ways_to_construct(line, patterns, cache);
        if (nways) {
            ++constructable_count;
        }
        part2_res += nways;
    }

    std::println("Part 1 result: {}", constructable_count);
    std::println("Part 2 result: {}", part2_res);

    return 0;
}

std::uint64_t count_ways_to_construct(const std::string& str,
    const std::vector<std::string>& patterns, std::map<std::string, std::uint64_t>& cache)
{
    auto [it, inserted] = cache.try_emplace(str, 0);
    if (!inserted) {
        return it->second;
    }

    std::uint64_t res {};
    for (const auto& pattern : patterns) {
        if (str.starts_with(pattern)) {
            if (str.length() == pattern.length()) {
                ++res;
                continue;
            }

            const auto suffix = str.substr(pattern.size());
            res += count_ways_to_construct(suffix, patterns, cache);
        }
    }

    it->second = res;
    return res;
}
