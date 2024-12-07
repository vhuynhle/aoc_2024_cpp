#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <vector>

/// Find x such that a = x || b
std::optional<int64_t> prefix(std::int64_t a, std::int64_t b)
{
    if (a < b) {
        return std::nullopt;
    }

    do {
        const auto d = b % 10;
        if ((a - d) % 10 != 0) {
            return std::nullopt;
        }
        a /= 10;
        b /= 10;
    } while (b);

    return a;
}

bool is_valid_equation(std::span<const int64_t> operands, int64_t target, bool use_concat)
{
    if (operands.empty()) {
        return false;
    }
    const auto operand = operands.front();

    if (operands.size() == 1) {
        if (operand == target) {
            std::print("{}", operand);
            return true;
        }
        return false;
    }

    if (operand <= target && is_valid_equation(operands.subspan(1), target - operand, use_concat)) {
        std::print(" + {}", operand);
        return true;
    }

    if (target % operand == 0
        && is_valid_equation(operands.subspan(1), target / operand, use_concat)) {
        std::print(" * {}", operand);
        return true;
    }

    if (use_concat) {
        auto p = prefix(target, operand);
        if (p.has_value() && is_valid_equation(operands.subspan(1), p.value(), use_concat)) {
            std::print("||{}", operand);
            return true;
        }
    }

    return false;
}

std::pair<int64_t, std::vector<int64_t>> parse_line(const std::string& line)
{
    std::stringstream ss { line };
    char colon;
    int64_t target;
    int64_t num;
    std::vector<int64_t> operands;
    ss >> target >> colon;
    while (ss >> num) {
        operands.push_back(num);
    }

    return { target, operands };
}

int main()
{
    std::string line;
    std::stringstream ss;

    std::int64_t part1_result {};
    std::int64_t part2_result {};
    while (std::getline(std::cin, line)) {
        auto [target, operands] = parse_line(line);
        std::reverse(operands.begin(), operands.end());

        if (is_valid_equation(operands, target, false)) {
            part1_result += target;
            part2_result += target;
            std::println(" --> {}", target);
        } else if (is_valid_equation(operands, target, true)) {
            part2_result += target;
            std::println(" --> {}", target);
        }
    }

    std::println("Part 1 result: {}", part1_result);
    std::println("Part 2 result: {}", part2_result);
    return 0;
}
