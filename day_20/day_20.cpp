#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <print>
#include <queue>
#include <set>
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

void find_cheats(std::span<const std::string> grid, std::span<const Coord> path,
    std::uint64_t max_cheat_length, std::uint64_t min_advantage)
{

    std::map<Coord, std::uint64_t> distances;
    for (std::uint64_t i { 0 }; i < path.size(); ++i) {
        distances[path[i]] = i;
    }

    const auto nrows = grid.size();
    const auto ncols = grid.front().size();

    std::uint64_t ncheats {};
    for (std::uint64_t d { 0 }; d < path.size(); ++d) {

        // bfs from the current point
        const Coord start = path[d];
        std::queue<std::pair<Coord, std::uint64_t>> q;
        std::set<Coord> visited {};
        std::set<Coord> cheat_ends {};
        q.emplace(start, 0);
        visited.emplace(start);

        while (!q.empty()) {
            const auto [current, current_distance] = q.front();
            q.pop();

            for (auto next : get_neighbors(current.first, current.second)) {
                const auto [nr, nc] = next;
                if ((nr >= nrows) || (nc >= ncols) || visited.contains(next)) {
                    continue;
                }

                const auto next_distance = current_distance + 1;

                if (grid[nr][nc] != '#') { // Check if this is a potential cheat end point
                    auto it = distances.find(next);
                    if (it != distances.end()) {
                        const auto nocheat_cost = it->second;
                        const auto cheat_cost = d + next_distance;
                        if (cheat_cost + min_advantage <= nocheat_cost) { // Beneficial cheat
                            cheat_ends.emplace(next);
                        }
                    }
                }

                // Continue the cheat
                if ((next_distance < max_cheat_length) && !visited.contains(next)) {
                    q.emplace(next, next_distance);
                    visited.emplace(next);
                }
            }
        }

        ncheats += cheat_ends.size();
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

    find_cheats(grid, path, min_advantage);

    find_cheats(grid, path, p2_max_cheat_length, min_advantage);
    return 0;
}
