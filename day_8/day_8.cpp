#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <string>
#include <vector>

static constexpr auto num_digits = 10;
static constexpr auto num_lowercases = 26;
static constexpr auto num_uppercases = num_lowercases;
static constexpr std::size_t n_alphanum = num_digits + num_lowercases + num_uppercases;

inline constexpr bool isalphanum(char ch)
{
    return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'z'))
        || ((ch >= 'A') && (ch <= 'Z'));
}

inline constexpr std::uint8_t char_index(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return static_cast<std::uint8_t>(ch - '0');
    } else if (ch >= 'a' && ch <= 'z') {
        return static_cast<std::uint8_t>(ch - 'a') + num_digits;
    } else {
        return static_cast<std::uint8_t>(ch - 'A') + num_digits + num_lowercases;
    }
}

struct Point {
    std::int64_t x; // row
    std::int64_t y; // col
};

static inline bool inside_rect(Point a, Point corner)
{
    return (a.x >= 0) && (a.x < corner.x) && (a.y >= 0) && (a.y < corner.y);
}

bool mark_antinode(std::vector<std::vector<std::uint8_t>>& arr, Point p)
{
    const Point corner { static_cast<std::int64_t>(arr.size()),
        static_cast<std::int64_t>(arr[0].size()) };

    if (inside_rect(p, corner)) {
        arr[static_cast<std::size_t>(p.x)][static_cast<std::size_t>(p.y)] = 1;
        return true;
    }
    return false;
}

void part1(const std::array<std::vector<Point>, n_alphanum>& node_map, std::size_t nrows,
    std::size_t ncols)
{
    std::vector<std::vector<std::uint8_t>> antinode_map(
        nrows, std::vector<std::uint8_t>(ncols, static_cast<std::uint8_t>(0)));

    for (const auto& antinodes : node_map) {
        for (std::size_t i { 0 }; i < antinodes.size(); ++i) {
            for (std::size_t j { i + 1 }; j < antinodes.size(); ++j) {
                const auto dx = antinodes[i].x - antinodes[j].x;
                const auto dy = antinodes[i].y - antinodes[j].y;
                mark_antinode(antinode_map, Point { antinodes[i].x + dx, antinodes[i].y + dy });
                mark_antinode(antinode_map, Point { antinodes[j].x - dx, antinodes[j].y - dy });
            }
        }
    }

    std::int64_t result { 0 };
    for (const auto& row : antinode_map) {
        result += std::count(row.begin(), row.end(), 1);
    }

    std::println("Part 1 result: {}", result);
}

void part2(const std::array<std::vector<Point>, n_alphanum>& node_map, std::size_t nrows,
    std::size_t ncols)
{
    std::vector<std::vector<std::uint8_t>> antinode_map(
        nrows, std::vector<std::uint8_t>(ncols, static_cast<std::uint8_t>(0)));

    for (const auto& antinodes : node_map) {
        for (std::size_t i { 0 }; i < antinodes.size(); ++i) {
            for (std::size_t j { i + 1 }; j < antinodes.size(); ++j) {
                const auto dx = antinodes[i].x - antinodes[j].x;
                const auto dy = antinodes[i].y - antinodes[j].y;
                Point p { antinodes[i] };
                while (mark_antinode(antinode_map, p)) {
                    p.x += dx;
                    p.y += dy;
                }

                p = antinodes[j];
                while (mark_antinode(antinode_map, p)) {
                    p.x -= dx;
                    p.y -= dy;
                }
            }
        }
    }

    std::int64_t result { 0 };
    for (const auto& row : antinode_map) {
        result += std::count(row.begin(), row.end(), 1);
    }

    std::println("Part 2 result: {}", result);
}

int main()
{
    std::array<std::vector<Point>, n_alphanum> map;
    std::size_t ncols { 0 };

    std::size_t row { 0 };
    std::string line;
    while (std::getline(std::cin, line)) {
        ncols = line.size();
        for (std::size_t col { 0 }; col < ncols; ++col) {
            const auto ch = line[col];
            if (isalphanum(ch)) {
                map[char_index(ch)].push_back(
                    Point(static_cast<std::int64_t>(row), static_cast<std::int64_t>(col)));
            }
        }
        ++row;
    }

    const auto nrows = row;
    part1(map, nrows, ncols);
    part2(map, nrows, ncols);

    return 0;
}
