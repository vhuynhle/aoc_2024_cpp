#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <print>
#include <queue>
#include <span>
#include <string>
#include <utility>
#include <vector>

static constexpr auto kDistanceLimit = std::numeric_limits<std::uint64_t>::max();

static inline std::array<std::pair<std::uint64_t, std::uint64_t>, 4> neighbors(
    std::uint64_t row, std::uint64_t col)
{
    return std::array<std::pair<std::uint64_t, std::uint64_t>, 4> {
        {
            { row - 1, col },
            { row + 1, col },
            { row, col - 1 },
            { row, col + 1 },
        },
    };
}

std::uint64_t bfs(std::span<const std::string> grid)
{
    struct Record {
        std::uint64_t row;
        std::uint64_t col;
        std::uint64_t distance;
    };

    const auto target_coord = grid.size() - 2; // Exclude padding
    auto q = std::queue<Record> {};
    q.emplace(1, 1, 0);

    auto distances = std::vector<std::vector<std::uint64_t>>(
        grid.size(), std::vector<std::uint64_t>(grid.size(), kDistanceLimit));

    while (!q.empty()) {
        const auto [r, c, distance] = q.front();
        q.pop();

        if (r == target_coord && c == target_coord) { // Found the path to the destination
            return distance;
        }

        for (auto [nr, nc] : neighbors(r, c)) {
            if ((grid[nr][nc] == '.') && (distances[nr][nc] == kDistanceLimit)) {
                distances[nr][nc] = distance + 1;
                q.emplace(nr, nc, distances[nr][nc]);
            }
        }
    }

    return kDistanceLimit;
}

std::vector<std::string> make_grid(std::vector<std::string> init_grid,
    std::span<const std::pair<std::uint64_t, std::uint64_t>> blocks, std::uint64_t block_count)
{
    for (auto [r, c] : blocks.subspan(0, block_count)) {
        init_grid[r][c] = '#';
    }
    return init_grid;
}

int main()
{
    std::uint64_t grid_size;
    std::uint64_t part1_limit;
    std::cin >> grid_size >> part1_limit;

    const auto row_border = std::string(grid_size + 2, '#');
    const auto row_inner = [grid_size]() -> std::string {
        auto row = std::string(grid_size + 2, '.');
        row.front() = '#';
        row.back() = '#';
        return row;
    }();

    std::vector<std::string> init_grid(grid_size + 2, row_inner);
    init_grid.front() = row_border;
    init_grid.back() = row_border;

    std::vector<std::pair<std::uint64_t, std::uint64_t>> blocks;
    std::uint64_t col;
    std::uint64_t row;
    char ignore;
    while ((std::cin >> col >> ignore >> row)) {
        blocks.emplace_back(row + 1, col + 1); // +1 for padding
    }

    // Part 1
    std::println("Part 1 result: {}", bfs(make_grid(init_grid, blocks, part1_limit)));

    // Part 2: binary search for the result
    const auto reachable = [&init_grid, &blocks](std::uint64_t block_count) {
        const auto grid = make_grid(init_grid, blocks, block_count);
        return bfs(grid) != kDistanceLimit;
    };

    auto lo = part1_limit - 1;
    auto hi = blocks.size() - 1;
    while (lo < hi) {
        const auto mid = lo + (hi - lo) / 2;
        if (reachable(mid)) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    const auto block_count = lo;
    const auto index = block_count - 1;
    const auto r = blocks[index].first - 1;  // -1 padding
    const auto c = blocks[index].second - 1; // -1 padding
    std::println("Part 2 result: index {}, (col,row)={},{}", index, c, r);

    return 0;
}
