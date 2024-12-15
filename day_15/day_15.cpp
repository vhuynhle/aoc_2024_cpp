#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <istream>
#include <optional>
#include <print>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

using Position = std::pair<std::uint64_t, std::uint64_t>;

std::vector<std::string> read_grid(std::istream& stream)
{
    std::vector<std::string> grid;
    std::string line;
    while ((std::getline(stream, line)) && (!line.empty())) {
        grid.push_back(line);
    }
    return grid;
}

std::string read_moves(std::istream& stream)
{
    std::string result {};
    std::string line;
    while (std::getline(stream, line)) {
        result.append(line);
    }

    return result;
}

Position find(std::span<const std::string> grid, char marker = '@')
{
    for (std::uint64_t row { 0 }; row < grid.size(); ++row) {
        const std::uint64_t col = grid[row].find(marker);
        if (col != std::string::npos) {
            return { row, col };
        }
    }

    throw std::invalid_argument("Robot not found");
}

static inline Position next_position(char direction, std::uint64_t row, std::uint64_t col)
{
    switch (direction) {
    case '^':
        return { row - 1, col };
    case '>':
        return { row, col + 1 };
    case 'v':
        return { row + 1, col };
    default:
        return { row, col - 1 };
    }
}

std::optional<Position> find_free_cell(
    char direction, std::uint64_t row, std::uint64_t col, std::span<const std::string> grid)
{
    while (true) {
        std::tie(row, col) = next_position(direction, row, col);

        if (grid[row][col] == '#') { // Hit a wall
            return std::nullopt;
        }

        if (grid[row][col] == '.') { // Free space found
            return Position { row, col };
        }
    }
}

std::uint64_t gps_score(std::span<const std::string> grid, char marker)
{
    const auto nrows = grid.size();
    const auto ncols = grid.front().size();
    std::uint64_t score {};
    for (std::size_t i { 1 }; i < nrows - 1; ++i) {
        for (std::size_t j { 1 }; j < ncols - 1; ++j) {
            if (grid[i][j] == marker) {
                score += (100 * i + j);
            }
        }
    }

    return score;
}

void print_grid(std::span<const std::string> grid)
{
    std::println();
    for (const auto& row : grid) {
        std::println("{}", row);
    }
    std::println();
}

void part_1(std::vector<std::string> grid, std::string_view moves)
{
    auto [row, col] = find(grid, '@');

    for (auto direction : moves) {
        auto free_cell = find_free_cell(direction, row, col, grid);
        if (!free_cell.has_value()) {
            continue;
        }

        // Shift the robot and all the blocks
        const auto [free_r, free_c] = *free_cell;
        const auto [next_r, next_c] = next_position(direction, row, col);
        if ((free_r != next_r) || (free_c != next_c)) {
            // Move the blocks between the robot and the free cell
            // In Part 1, all the blocks are the same 'O' so we just need to mark
            // one cell with 'O'.
            grid[free_r][free_c] = 'O';
        }
        grid[next_r][next_c] = '@';
        grid[row][col] = '.';

        row = next_r;
        col = next_c;
    }

    print_grid(grid);
    std::println("Part 1 score: {}", gps_score(grid, 'O'));
}

std::vector<std::string> double_grid(std::span<const std::string> grid)
{
    const std::uint64_t nrows { grid.size() };
    const std::uint64_t ncols { grid.front().size() };
    std::vector<std::string> res(nrows, std::string(2 * ncols, ' '));
    for (std::uint64_t i { 0 }; i < nrows; ++i) {
        for (std::uint64_t j { 0 }; j < ncols; ++j) {
            switch (grid[i][j]) {
            case '@':
                res[i][2 * j] = '@';
                res[i][2 * j + 1] = '.';
                break;
            case '#':
                res[i][2 * j] = '#';
                res[i][2 * j + 1] = '#';
                break;
            case 'O':
                res[i][2 * j] = '[';
                res[i][2 * j + 1] = ']';
                break;
            default:
                res[i][2 * j] = '.';
                res[i][2 * j + 1] = '.';
                break;
            }
        }
    }

    return res;
}

void part_2(std::span<const std::string> orig_grid, std::string_view moves)
{
    using std::ranges::contains;

    auto grid = double_grid(orig_grid);
    auto [current_row, current_col] = find(orig_grid, '@');
    current_col *= 2;

    for (char direction : moves) {
        if (direction == '<' || direction == '>') { // Move horizontally, the same method as Part 1
            auto free_cell = find_free_cell(direction, current_row, current_col, grid);
            if (!free_cell.has_value()) {
                continue;
            }

            const auto free_col = free_cell->second;
            if (direction == '<') {
                std::memmove(&grid[current_row][free_col], &grid[current_row][free_col + 1],
                    current_col - free_col);
            } else {
                std::memmove(&grid[current_row][current_col + 1], &grid[current_row][current_col],
                    free_col - current_col);
            }
            grid[current_row][current_col] = '.';

            std::tie(current_row, current_col) = next_position(direction, current_row, current_col);
        } else { // Move vertically

            // Check if the move is possible and collect the cells that need to move
            bool movable { true };
            std::queue<Position> cells_in_progress {};
            std::vector<Position> cells_to_move {};
            cells_in_progress.emplace(current_row, current_col);
            cells_to_move.emplace_back(current_row, current_col);
            while (!cells_in_progress.empty()) {
                const auto [r, c] = cells_in_progress.front();
                cells_in_progress.pop();

                const auto [next_r, next_c] = next_position(direction, r, c);

                if (grid[next_r][next_c] == '.') {
                    continue;
                }

                if (grid[next_r][next_c] == '#') {
                    movable = false;
                    break;
                }

                if (!contains(cells_to_move, std::make_pair(next_r, next_c))) {
                    cells_in_progress.emplace(next_r, next_c);
                    cells_to_move.emplace_back(next_r, next_c);
                }
                if (grid[next_r][next_c] == '['
                    && !contains(cells_to_move, std::make_pair(next_r, next_c + 1))) {
                    cells_to_move.emplace_back(next_r, next_c + 1);
                    cells_in_progress.emplace(next_r, next_c + 1);
                } else if (grid[next_r][next_c] == ']'
                    && !contains(cells_to_move, std::make_pair(next_r, next_c - 1))) {
                    cells_to_move.emplace_back(next_r, next_c - 1);
                    cells_in_progress.emplace(next_r, next_c - 1);
                }
            }

            if (!movable) {
                continue;
            }

            // Move the cells furthest first to avoid overwriting cells that have not been moved
            for (auto it = cells_to_move.rbegin(); it != cells_to_move.rend(); ++it) {
                const auto [r, c] = *it;
                const auto [next_r, next_c] = next_position(direction, r, c);
                grid[next_r][next_c] = grid[r][c];
                grid[r][c] = '.';
            }

            std::tie(current_row, current_col) = next_position(direction, current_row, current_col);
        }
    }

    print_grid(grid);
    std::println("Part 2 score: {}", gps_score(grid, '['));
}

int main()
{
    auto grid = read_grid(std::cin);
    const auto moves = read_moves(std::cin);

    part_1(grid, moves);
    part_2(grid, moves);

    return 0;
}
