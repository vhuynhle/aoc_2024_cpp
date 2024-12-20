#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <print>
#include <queue>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using Coord = std::pair<std::uint64_t, std::uint64_t>;

std::tuple<std::vector<std::string>, Coord, Coord> read_input()
{
    std::vector<std::string> grid;
    Coord start;
    Coord end;

    std::string line;
    std::uint64_t row { 0 };
    bool found_start { false };
    bool found_end { false };
    uint64_t pos { 0 };
    while (std::getline(std::cin, line)) {
        grid.push_back(line);

        if (!found_start && ((pos = line.find('S')) != std::string::npos)) {
            start.first = row;
            start.second = pos;
        }

        if (!found_end && ((pos = line.find('E')) != std::string::npos)) {
            end.first = row;
            end.second = pos;
        }

        ++row;
    }
    return { grid, start, end };
}

static inline std::array<Coord, 4> get_neighbors(std::uint64_t row, std::uint64_t col)
{
    return { {
        { row, col - 1 },
        { row, col + 1 },
        { row - 1, col },
        { row + 1, col },
    } };
}

std::vector<Coord> find_path(std::span<const std::string> grid, Coord start, Coord end)
{
    const std::uint64_t nrows = grid.size();
    const std::uint64_t ncols = grid.front().size();

    std::vector<std::vector<Coord>> prev(nrows, std::vector<Coord>(ncols));
    std::vector<std::vector<std::uint8_t>> visited(nrows, std::vector<std::uint8_t>(ncols, 0));
    std::queue<Coord> q;
    visited[start.first][start.second] = 1;

    q.emplace(start);
    while (!q.empty()) {
        const auto current = q.front();
        const auto [row, col] = current;
        q.pop();
        if (current == end) {
            break;
        }

        for (auto [nr, nc] : get_neighbors(row, col)) {
            if (grid[nr][nc] != '#' && !visited[nr][nc]) {
                q.emplace(nr, nc);
                prev[nr][nc] = current;
                visited[nr][nc] = 1;
            }
        }
    }

    if (!visited[end.first][end.second]) {
        throw std::invalid_argument { "Path not found" };
    }

    auto res = std::vector<Coord> {};
    Coord c = end;
    res.push_back(c);
    while (c != start) {
        c = prev[c.first][c.second];
        res.push_back(c);
    }

    std::reverse(res.begin(), res.end());
    return res;
}

void find_cheats(
    std::span<const std::string> grid, std::span<const Coord> path, std::uint64_t min_advantage)
{

    std::map<Coord, std::uint64_t> distances;
    for (std::uint64_t i { 0 }; i < path.size(); ++i) {
        distances[path[i]] = i;
    }

    const auto nrows = grid.size();
    const auto ncols = grid.front().size();

    std::uint64_t ncheats {};
    for (std::uint64_t d { 0 }; d < path.size(); ++d) {
        const auto [row, col] = path[d];
        const auto potential_clips = std::array<Coord, 4> { {
            { row, col - 2 },
            { row, col + 2 },
            { row - 2, col },
            { row + 2, col },
        } };
        for (auto clip : potential_clips) {
            if ((clip.first < nrows) && (clip.second < ncols)) {
                auto it = distances.find(clip);
                if ((it != distances.end()) && (it->second >= d + min_advantage + 2)) {
                    // Cheat found!
                    ++ncheats;
                }
            }
        }
    }

    std::println("Number of cheats: {}", ncheats);
}

std::uint64_t manhattan_distance(Coord d1, Coord d2)
{
    const auto [x1, y1] = d1;
    const auto [x2, y2] = d2;
    const auto dx = (x1 >= x2) ? (x1 - x2) : (x2 - x1);
    const auto dy = (y1 >= y2) ? (y1 - y2) : (y2 - y1);
    return dx + dy;
}

void find_cheats(
    std::span<const Coord> path, std::uint64_t max_cheat_length, std::uint64_t min_advantage)
{
    std::uint64_t ncheats {};
    for (std::uint64_t d1 { 0 }; d1 < path.size(); ++d1) {
        for (std::uint64_t d2 { d1 + min_advantage + 2 }; d2 < path.size(); ++d2) {
            const auto cheat_distance = manhattan_distance(path[d1], path[d2]);
            const auto advantage = (d2 - d1) - cheat_distance;
            ncheats += ((cheat_distance <= max_cheat_length) && (advantage >= min_advantage));
        }
    }

    std::println("Number of cheats: {}", ncheats);
}

template <typename T>
using Matrix = std::vector<std::vector<T>>;

void find_cheats_fast(std::span<const Coord> path, std::uint64_t max_cheat_length,
    std::uint64_t min_advantage, std::uint64_t nrows, std::uint64_t ncols)
{
    // Divide the big grid of size (nrows x ncols) into grid of size ((max_cheat_length + 1) x
    // (max_cheat_length + 1))
    // Only points in the same or neighboring grids need to be considered as start and end points
    // of cheats

    struct Record {
        Coord coord;
        std::uint64_t distance;
    };
    using RecordList = std::vector<Record>;

    const auto group_nrows = nrows / (max_cheat_length + 1) + 1;
    const auto group_ncols = ncols / (max_cheat_length + 1) + 1;
    Matrix<RecordList> groups(group_ncols, std::vector<RecordList>(group_ncols, RecordList {}));

    for (std::uint64_t distance { 0 }; distance < path.size(); ++distance) {
        const auto coord = path[distance];
        const auto group_row = coord.first / (max_cheat_length + 1);
        const auto group_col = coord.second / (max_cheat_length + 1);
        groups[group_row][group_col].emplace_back(coord, distance);
    }

    const auto surrounding_groups = [](std::uint64_t i, std::uint64_t j) {
        return std::array<std::pair<std::uint64_t, std::uint64_t>, 9> { {
            { i - 1, j - 1 },
            { i - 1, j },
            { i - 1, j + 1 },
            { i, j - 1 },
            { i, j },
            { i, j + 1 },
            { i + 1, j - 1 },
            { i + 1, j },
            { i + 1, j + 1 },
        } };
    };

    std::uint64_t ncheats {};
    for (std::uint64_t i { 0 }; i < groups.size(); ++i) {
        for (std::uint64_t j { 0 }; j < groups.front().size(); ++j) {
            for (auto [coord1, distance1] : groups[i][j]) { // Iterate over the records in a grid

                // Consider records in neighboring grids as cheat candidate
                for (auto [ni, nj] : surrounding_groups(i, j)) {
                    if ((ni >= group_nrows) || (nj >= group_ncols)) {
                        continue;
                    }

                    for (auto [coord2, distance2] : groups[ni][nj]) {
                        if (distance2 >= distance1 + min_advantage + 2) {
                            const auto cheat_distance = manhattan_distance(coord1, coord2);
                            const auto advantage = (distance2 - distance1) - cheat_distance;
                            ncheats += ((cheat_distance <= max_cheat_length)
                                && (advantage >= min_advantage));
                        }
                    }
                }
            }
        }
    }

    std::println("Number of cheats: {}", ncheats);
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        const char* prog_name = (argc > 0) ? argv[0] : "<program_name>";
        std::println(std::cerr, "Usage: {} <min advantage> <part 2 max cheat length>", prog_name);
        return EXIT_FAILURE;
    }

    const std::uint64_t min_advantage = std::strtoull(argv[1], nullptr, 10);
    const std::uint64_t p2_max_cheat_length = std::strtoull(argv[2], nullptr, 10);

    const auto [grid, start, end] = read_input();
    const auto path = find_path(grid, start, end);
    std::println("Path length: {}", path.size());

    find_cheats(grid, path, min_advantage);
    // find_cheats(path, p2_max_cheat_length, min_advantage);
    find_cheats_fast(path, p2_max_cheat_length, min_advantage, grid.size(), grid.front().size());
    return 0;
}
