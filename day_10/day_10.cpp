#include <algorithm>
#include <cstdint>
#include <iostream>
#include <print>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Point {
    std::uint64_t row;
    std::uint64_t col;
};

bool operator<(Point p1, Point p2)
{
    return (p1.row < p2.row) || ((p1.row == p2.row) && (p1.col < p2.col));
}

using PointSet = std::set<Point>;

template <typename T>
using mat = std::vector<std::vector<T>>;

static std::uint8_t digit_to_int(char digit)
{
    if (digit < '0' || digit > '9') {
        throw std::invalid_argument("Not a valid height");
    }
    return static_cast<std::uint8_t>(digit - '0');
}

static mat<uint8_t> read_input()
{
    bool first_line { true };
    std::string line;
    std::uint64_t nrows {};
    std::uint64_t ncols {};
    mat<std::uint8_t> map;

    while (std::getline(std::cin, line)) {
        if (first_line) {
            ncols = line.length();
            first_line = false;
        } else if (ncols != line.length()) {
            throw std::invalid_argument("Inconsistent line length");
        }

        std::vector<std::uint8_t> heights(line.length(), 0);
        std::transform(line.begin(), line.end(), heights.begin(), digit_to_int);
        map.push_back(std::move(heights));
        ++nrows;
    }

    if (nrows == 0 || ncols == 0) {
        throw std::invalid_argument("Empty input");
    }

    return map;
}

static mat<Point> get_map_by_height(const mat<std::uint8_t>& map)
{
    static constexpr std::uint64_t num_heights { 10 };
    mat<Point> height_to_coords(num_heights, std::vector<Point> {});
    const std::uint64_t nrows = map.size();
    const std::uint64_t ncols = map.front().size();

    for (std::uint64_t i { 0 }; i < nrows; ++i) {
        for (std::uint64_t j { 0 }; j < ncols; ++j) {
            const auto height = static_cast<std::uint64_t>(map[i][j]);
            height_to_coords[height].emplace_back(i, j);
        }
    }

    return height_to_coords;
}

static std::vector<Point> neighbors(std::uint64_t nrows, std::uint64_t ncols, Point p)
{
    std::vector<Point> res;

    if (p.row > 0) {
        res.emplace_back(p.row - 1, p.col);
    }

    if (p.row + 1 < nrows) {
        res.emplace_back(p.row + 1, p.col);
    }

    if (p.col > 0) {
        res.emplace_back(p.row, p.col - 1);
    }

    if (p.col + 1 < ncols) {
        res.emplace_back(p.row, p.col + 1);
    }

    return res;
}

int main()
{
    const auto map = read_input();
    const std::uint64_t nrows = map.size();
    const std::uint64_t ncols = map.front().size();
    const auto height_to_coords = get_map_by_height(map);

    mat<PointSet> reachable_endpoints(nrows, std::vector<PointSet>(ncols, PointSet {}));
    mat<std::uint64_t> ratings { nrows, std::vector<std::uint64_t>(ncols, 0) };

    // Init: Reachable endpoints from the endpoints themselves
    for (auto [row, col] : height_to_coords[9]) {
        reachable_endpoints[row][col].emplace(row, col);
        ratings[row][col] = 1;
    }

    // Calculate the reachable destinations and the rating for each
    // point of height (i - 1); i = 9..1
    for (std::uint8_t height = 9; height > 0; --height) {
        for (auto p : height_to_coords[height - 1]) {
            for (auto n : neighbors(nrows, ncols, p)) {
                if (map[n.row][n.col] == height) {
                    reachable_endpoints[p.row][p.col].insert(
                        reachable_endpoints[n.row][n.col].begin(),
                        reachable_endpoints[n.row][n.col].end());
                    ratings[p.row][p.col] += ratings[n.row][n.col];
                }
            }
        }
    }

    // Count the total reachable destinations and the ratings for points of height 0
    std::uint64_t total_reachable { 0 };
    std::uint64_t total_ratings { 0 };
    for (std::uint64_t r { 0 }; r < nrows; ++r) {
        for (std::uint64_t c { 0 }; c < ncols; ++c) {
            if (map[r][c] == 0) {
                total_reachable += reachable_endpoints[r][c].size();
                total_ratings += ratings[r][c];
            }
        }
    }

    std::println("Part 1 score: {}", total_reachable);
    std::println("Part 2 score: {}", total_ratings);

    return 0;
}
