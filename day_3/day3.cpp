#include <cstdint>
#include <iostream>
#include <regex>
#include <string>

std::int64_t parse_line(const std::string& line)
{
    std::regex mul_regex { "mul\\((\\d+),(\\d+)\\)" };
    std::smatch match;
    std::int64_t sum { 0 };

    auto start = line.begin();
    auto end = line.end();
    while (true) {
        std::regex_search(start, end, match, mul_regex);
        if (match.empty()) {
            break;
        }
        const int64_t a = std::stoll(match.str(1));
        const int64_t b = std::stoll(match.str(2));
        sum += (a * b);
        start = match[0].second;
    }

    return sum;
}

std::int64_t parse_line_v2(const std::string& line, bool& enabled)
{
    std::regex mul_regex { "mul\\((\\d+),(\\d+)\\)|do\\(\\)|don't\\(\\)" };
    std::smatch match;
    std::int64_t sum { 0 };

    auto start = line.begin();
    auto end = line.end();
    while (true) {
        std::regex_search(start, end, match, mul_regex);
        if (match.empty()) {
            break;
        }
        if (match.str() == "do()") {
            enabled = true;
        } else if (match.str() == "don't()") {
            enabled = false;
        } else if (enabled) {
            const int64_t a = std::stoll(match.str(1));
            const int64_t b = std::stoll(match.str(2));
            sum += (a * b);
        }
        start = match[0].second;
    }

    return sum;
}

int main()
{
    std::string line;

    bool active { true };
    std::int64_t sum_p1 { 0 };
    std::int64_t sum_p2 { 0 };
    while (std::getline(std::cin, line)) {
        sum_p1 += parse_line(line);
        sum_p2 += parse_line_v2(line, active);
    }

    std::cout << "Part 1: " << sum_p1 << std::endl;
    std::cout << "Part 2: " << sum_p2 << std::endl;

    return 0;
}
