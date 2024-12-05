#include <algorithm>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::pair<std::int64_t, std::int64_t> parse_rule(std::string_view line)
{
    std::int64_t a;
    std::int64_t b;
    const auto bar_pos = line.find('|');
    std::from_chars(line.data(), line.data() + bar_pos, a);
    std::from_chars(line.data() + bar_pos + 1, line.data() + line.length(), b);
    return { a, b };
}

std::set<std::pair<std::int64_t, std::int64_t>> parse_rules(std::istream& ifs)
{
    std::string line {};
    std::set<std::pair<std::int64_t, std::int64_t>> rules;
    while (true) {
        std::getline(ifs, line);
        if (line.empty()) {
            break;
        }
        rules.insert(parse_rule(line));
    }
    return rules;
}

std::vector<std::int64_t> parse_update(std::string_view line)
{
    std::vector<std::int64_t> result {};
    std::string_view::size_type pos { 0 };

    while (pos != std::string_view::npos) {
        const auto commas_pos = [&line, pos]() {
            const auto res = line.find(',', pos);
            return res != std::string_view::npos ? res : line.size();
        }();

        std::int64_t val { 0 };
        std::from_chars(line.data() + pos, line.data() + commas_pos, val);
        result.push_back(val);

        pos = commas_pos < line.size() ? commas_pos + 1 : std::string_view::npos;
    }

    return result;
}

int main()
{
    const auto rules = parse_rules(std::cin);
    const auto cmp = [&rules](std::int64_t a, std::int64_t b) -> bool {
        return rules.contains(std::make_pair(a, b));
    };

    std::string line;
    std::int64_t sum_p1 { 0 };
    std::int64_t sum_p2 { 0 };
    while (std::getline(std::cin, line)) {
        auto update = parse_update(line);

        if (std::ranges::is_sorted(update, cmp)) {
            sum_p1 += update[update.size() / 2];
        } else {
            std::ranges::sort(update, cmp);
            sum_p2 += update[update.size() / 2];
        }
    }

    std::cout << "Part 1 answer: " << sum_p1 << std::endl;
    std::cout << "Part 2 answer: " << sum_p2 << std::endl;

    return 0;
}
