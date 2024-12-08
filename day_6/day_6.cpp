#include <algorithm>
#include <cstdint>
#include <iostream>
#include <print>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static inline constexpr std::uint8_t encode_direction(char direction)
{
    switch (direction) {
    case '^':
        return 1 << 0;
    case '>':
        return 1 << 1;
    case 'v':
        return 1 << 2;
    default:
        return 1 << 3;
    }
}

static inline constexpr char change_direction(char direction)
{
    switch (direction) {
    case '^':
        return '>';
    case '>':
        return 'v';
    case 'v':
        return '<';
    default:
        return '^';
    }
}

static inline std::pair<std::int64_t, std::int64_t> next(
    std::uint64_t row, std::uint64_t col, char direction)
{
    const auto srow = static_cast<int64_t>(row);
    const auto scol = static_cast<int64_t>(col);
    switch (direction) {
    case '^':
        return { srow - 1, scol };
    case '>':
        return { srow, scol + 1 };
    case 'v':
        return { srow + 1, scol };
    default:
        return { srow, scol - 1 };
    }
}

static inline bool been_there(const std::vector<std::vector<std::uint8_t>>& records,
    std::uint64_t row, std::uint64_t col, char direction)
{
    const auto ed = encode_direction(direction);
    return records[row][col] & ed;
}

static inline bool has_obtacle(
    const std::vector<std::string>& map, std::uint64_t row, std::uint64_t col)
{
    return map[row][col] == '#';
}

static inline bool out_of_bound(
    std::uint64_t nrows, std::uint64_t ncols, std::int64_t row, std::int64_t col)
{
    return (row < 0) || (static_cast<uint64_t>(row) >= nrows) || (col < 0)
        || (static_cast<uint64_t>(col) >= ncols);
}

struct TraceResult {
    bool has_loop;
    std::vector<std::vector<std::uint8_t>> records;
};

TraceResult trace(const std::vector<std::string>& map, std::uint64_t row, std::uint64_t col)
{
    const auto nrows = map.size();
    const auto ncols = map[0].size();
    std::vector<std::vector<std::uint8_t>> records(nrows, std::vector<std::uint8_t>(ncols, 0));

    char direction = map[row][col];
    while (true) {
        if (been_there(records, row, col, direction)) {
            return { true, records };
        }

        records[row][col] |= encode_direction(direction);

        constexpr std::uint64_t max_direction_changes { 4 };
        for (std::uint64_t i {}; i < max_direction_changes; ++i) {
            auto [snext_row, snext_col] = next(row, col, direction);

            if (out_of_bound(nrows, ncols, snext_row, snext_col)) {
                return { false, records };
            }

            const auto next_row = static_cast<uint64_t>(snext_row);
            const auto next_col = static_cast<uint64_t>(snext_col);
            if (has_obtacle(map, next_row, next_col)) {
                direction = change_direction(direction);
            } else {
                row = next_row;
                col = next_col;
                break;
            }
        }
    }
}

std::pair<std::uint64_t, std::uint64_t> find_starting_position(std::span<const std::string> map)
{
    const auto nrows = map.size();
    const auto ncols = map[0].size();

    for (std::uint64_t i {}; i < nrows; ++i) {
        for (std::uint64_t j = 0; j < ncols; ++j) {
            const auto ch = map[i][j];
            if (ch == '^' || ch == '>' || ch == 'v' || ch == '<') {
                return { i, j };
            }
        }
    }

    throw std::invalid_argument("Startinng position not found");
}

int main()
{
    std::vector<std::string> map;
    std::string line;
    while (std::getline(std::cin, line)) {
        map.push_back(line);
    }
    const auto [init_row, init_col] = find_starting_position(map);

    // Part 1
    std::int64_t part1_res {};
    const auto records = trace(map, init_row, init_col).records;
    for (const auto& row : records) {
        part1_res += std::count_if(row.begin(), row.end(), [](auto val) { return val != 0; });
    }
    std::println("Part 1 result: {}", part1_res);

    // Part 2, brute-force solution
    std::int64_t part2_res {};
    for (std::uint64_t i {}; i < map.size(); ++i) {
        for (std::uint64_t j {}; j < map[0].size(); ++j) {
            // only put obstacle on an empty block on the original path, otherwise there's no change
            // in the trace
            if ((map[i][j] == '.') && records[i][j]) {
                map[i][j] = '#';
                if (trace(map, init_row, init_col).has_loop) {
                    ++part2_res;
                }

                // Remove the obstacle before trying with a new position
                map[i][j] = '.';
            }
        }
    }

    std::println("Part 2 result: {}", part2_res);

    return 0;
}
