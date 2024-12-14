#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

std::optional<std::pair<std::int64_t, std::int64_t>> parse_line(
    std::string_view line, char before, char after);

std::optional<std::pair<std::int64_t, std::int64_t>> solve_linear_equation(std::int64_t a1,
    std::int64_t b1, std::int64_t c1, std::int64_t a2, std::int64_t b2, std::int64_t c2);

int main()
{
    std::int64_t cost_p1 {};
    std::int64_t cost_p2 {};

    std::string line;
    while (std::getline(std::cin, line)) {
        const auto [a1, a2] = parse_line(line, '+', ',').value();

        std::getline(std::cin, line);
        if (!std::cin) {
            throw std::invalid_argument("No button B setting");
        }
        const auto [b1, b2] = parse_line(line, '+', ',').value();

        std::getline(std::cin, line);
        if (!std::cin) {
            throw std::invalid_argument("No prize setting");
        }
        const auto [c1, c2] = parse_line(line, '=', ',').value();

        auto solution_p1 = solve_linear_equation(a1, b1, c1, a2, b2, c2);
        if (solution_p1.has_value()) {
            const auto [x, y] = solution_p1.value();
            cost_p1 += (x * 3 + y);
        }

        static constexpr std::int64_t offset { 10000000000000 };
        auto solution_p2 = solve_linear_equation(a1, b1, c1 + offset, a2, b2, c2 + offset);
        if (solution_p2.has_value()) {
            const auto [x, y] = solution_p2.value();
            cost_p2 += (x * 3 + y);
        }

        std::getline(std::cin, line); // Discard empty line
    }

    std::println("Part 1 solution: {}", cost_p1);
    std::println("Part 2 solution: {}", cost_p2);

    return 0;
}

std::optional<std::pair<std::int64_t, std::int64_t>> parse_line(
    std::string_view line, char before, char after)
{
    std::uint64_t before_pos { line.find(before, 0) };
    if (before_pos == std::string_view::npos) {
        return std::nullopt;
    }

    std::uint64_t after_pos { line.find(after, before_pos + 1) };
    if (after_pos == std::string_view::npos) {
        return std::nullopt;
    }

    std::int64_t a;
    auto ret = std::from_chars(line.data() + (before_pos + 1), line.data() + after_pos, a);
    if ((ret.ec != std::errc {}) || (ret.ptr != line.data() + after_pos)) {
        return std::nullopt;
    }

    before_pos = line.find(before, after_pos + 1);
    if (before_pos == std::string::npos) {
        return std::nullopt;
    }

    std::uint64_t b;
    ret = std::from_chars(line.data() + before_pos + 1, line.data() + line.size(), b);
    if ((ret.ec != std::errc {} || (ret.ptr != line.data() + line.size()))) {
        return std::nullopt;
    }

    return std::make_pair(a, b);
}

std::optional<std::pair<std::int64_t, std::int64_t>> solve_linear_equation(std::int64_t a1,
    std::int64_t b1, std::int64_t c1, std::int64_t a2, std::int64_t b2, std::int64_t c2)
{
    const auto dc = a1 * b2 - a2 * b1;
    const auto da = c1 * b2 - c2 * b1;
    const auto db = a1 * c2 - a2 * c1;

    if (dc == 0) {
        throw std::runtime_error("Not handled: determinant = 0");
    }

    if (da % dc == 0 && db % dc == 0) {
        auto x = da / dc;
        auto y = db / dc;

        if ((x >= 0) && (y >= 0)) {
            return std::make_pair(x, y);
        }
        return std::nullopt;
    }
    return std::nullopt;
}
