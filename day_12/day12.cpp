#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <print>
#include <stack>
#include <string>
#include <utility>
#include <vector>

template <typename T>
using matrix = std::vector<std::vector<T>>;

enum class FillStatus {
    NotFilled,
    InProgress,
    Done,
};

struct PointType {
    bool topleft;
    bool topright;
    bool bottomleft;
    bool bottomright;
};

std::uint64_t corner_point(PointType ct) {
    // Each point can belongs to up to squares. The squares can be used to classify the point.
    // In the figure below, the point in question is marked by 'o'
    //
    // Case 1: The point belongs to 4 squares -> It's not a corner.
    // +-+-+
    // | | |
    // +-o-+
    // | | |
    // +-+-+
    //
    // Case 2: The point belongs to 1 or 3 squares -> It's a corner
    // +-+
    // | |
    // +-o
    //
    // +-+
    // | |
    // +-o-+
    // | | |
    // +-+-+
    //
    // Case 3: The point belongs to 2 squares that line up -> It's not a corner.
    // +-o-+
    // | | |
    // +-+-+
    //
    // Case 4: The point belongs 2 squares that sit diagonally -> It's a corner of 2 edges.
    // +-+
    // | |
    // +-o-+
    //   | |
    //   +-+
    const std::uint64_t num_squares =
        static_cast<std::uint64_t>(ct.topleft) + static_cast<std::uint64_t>(ct.topright) +
        static_cast<std::uint64_t>(ct.bottomleft) + static_cast<std::uint64_t>(ct.bottomright);

    switch (num_squares) {
        case 1:
        case 3:
            return 1;
        case 2:
            if ((ct.topleft && ct.bottomright) || (ct.topright && ct.bottomleft)) {
                return 2;
            }
            return 0;
        default:
            return 0;
    }
};

std::uint64_t count_edges(
    const std::map<std::pair<std::uint64_t, std::uint64_t>, PointType>& points) {
    // The number of edges of an area is the same as the number of corners
    return std::accumulate(points.begin(), points.end(), 0ULL,
                           [](auto init, auto p) { return init + corner_point(p.second); });
};

int main() {
    std::vector<std::string> image{};

    std::string line;
    while (std::getline(std::cin, line)) {
        image.push_back(line);
    }
    const std::uint64_t nrows{image.size()};
    const std::uint64_t ncols{image.front().size()};

    matrix<FillStatus> filled(nrows, std::vector<FillStatus>(ncols, FillStatus::NotFilled));

    std::uint64_t score_1{0};
    std::uint64_t score_2{0};
    for (std::uint64_t i{0}; i < nrows; ++i) {
        for (std::uint64_t j{0}; j < ncols; ++j) {
            if (filled[i][j] != FillStatus::NotFilled) {
                continue;
            }
            // A new area detected

            std::uint64_t perimeter{0};
            std::uint64_t area{0};
            const char color = image[i][j];
            filled[i][j] = FillStatus::InProgress;
            std::map<std::pair<std::uint64_t, std::uint64_t>, PointType> corners;

            std::stack<std::pair<std::uint64_t, std::uint64_t>> in_progress;
            in_progress.emplace(i, j);
            while (!in_progress.empty()) {
                const auto [r, c] = in_progress.top();
                in_progress.pop();
                filled[r][c] = FillStatus::Done;
                std::uint64_t filled_neighbors{};

                corners[std::make_pair(r, c)].topleft = true;
                corners[std::make_pair(r, c + 1)].topright = true;
                corners[std::make_pair(r + 1, c)].bottomleft = true;
                corners[std::make_pair(r + 1, c + 1)].bottomright = true;

                static constexpr std::array<std::pair<std::uint64_t, std::uint64_t>, 4> dxdys = {{
                    {0, std::numeric_limits<std::uint64_t>::max()},  // {0, -1}
                    {0, 1},
                    {std::numeric_limits<std::uint64_t>::max(), 0},  // {-1, 0}
                    {1, 0},
                }};

                for (auto [dx, dy] : dxdys) {
                    const auto u = r + dx;
                    const auto v = c + dy;

                    if ((u < nrows) && (v < ncols) && image[u][v] == color) {
                        if (filled[u][v] == FillStatus::NotFilled) {
                            in_progress.emplace(u, v);
                            filled[u][v] = FillStatus::InProgress;
                        } else if (filled[u][v] == FillStatus::Done) {
                            ++filled_neighbors;
                        }
                    }
                }

                ++area;
                switch (filled_neighbors) {
                    case 0:
                        perimeter += 4;
                        break;
                    case 1:
                        perimeter += 2;
                        break;
                    case 2:
                        break;
                    case 3:
                        perimeter -= 2;
                        break;
                    default:  // 4
                        perimeter -= 4;
                        break;
                }
            }

            score_1 += area * perimeter;
            const std::uint64_t edges{count_edges(corners)};
            score_2 += area * edges;
        }
    }

    std::println("Part 1 result: {}", score_1);
    std::println("Part 2 result: {}", score_2);

    return 0;
}
