#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline constexpr std::uint8_t encode_direction(char direction)
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

inline constexpr char change_direction(char direction)
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

inline std::pair<std::int64_t, std::int64_t> next(
    std::int64_t row, std::int64_t col, char direction)
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

bool been_there(const std::vector<std::vector<std::uint8_t>>& records, std::int64_t row,
    std::int64_t col, char direction)
{
    const auto ed = encode_direction(direction);
    return records[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] & ed;
}

bool has_obtacle(const std::vector<std::string>& map, std::int64_t row, std::int64_t col)
{
    return (row >= 0) && (static_cast<std::size_t>(row) < map.size()) && (col >= 0)
        && (static_cast<std::size_t>(col) < map[0].size())
        && (map[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] == '#');
}

bool out_of_bound(std::int64_t nrows, std::int64_t ncols, std::int64_t row, std::int64_t col)
{
    return (row < 0) || (col < 0) || (row >= nrows) || (col >= ncols);
}

int main()
{
    std::vector<std::string> map;
    std::string line;
    while (std::getline(std::cin, line)) {
        map.push_back(line);
    }

    const std::int64_t nrows = static_cast<std::int64_t>(map.size());
    const std::int64_t ncols = static_cast<std::int64_t>(map[0].size());
    std::int64_t row {};
    std::int64_t col {};
    char direction { '>' };
    bool found { false };
    for (std::size_t i { 0 }; i < map.size() && !found; ++i) {
        const auto& line = map[i];
        for (std::size_t j = 0; j < map[i].length() && !found; ++j) {
            if (line[j] == '^' || line[j] == '>' || line[j] == 'v' || line[j] == '<') {
                row = static_cast<std::int64_t>(i);
                col = static_cast<std::int64_t>(j);
                direction = line[j];
                found = true;
            }
        }
    }

    if (!found) {
        throw std::invalid_argument("Position not found");
    }

    std::vector<std::vector<std::uint8_t>> records(
        map.size(), std::vector<std::uint8_t>(map[0].length(), 0));

    while (true) {
        const std::uint8_t ed = encode_direction(direction);
        records[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] |= ed;

        // std::cout << "Current board: " << std::endl;
        // for (std::size_t i { 0 }; i < map.size(); ++i) {
        //     for (std::size_t j { 0 }; j < map[0].size(); ++j) {
        //         if (map[i][j] == '#') {
        //             std::cout << '#';
        //         } else if (records[i][j] == 0) {
        //             std::cout << ' ';
        //         } else {
        //             std::cout << (int)records[i][j];
        //         }
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << std::endl;

        bool has_next { false };
        for (std::size_t i { 0 }; i < 4; ++i) {
            auto [next_row, next_col] = next(row, col, direction);

            if (out_of_bound(nrows, ncols, next_row, next_col)) {
                break;
            }

            if (been_there(records, next_row, next_col, direction)) {
                break;
            }

            if (has_obtacle(map, next_row, next_col)) {
                direction = change_direction(direction);
            } else {
                row = next_row;
                col = next_col;
                has_next = true;
                break;
            }
        }

        if (!has_next) {
            break;
        }
    }

    std::int64_t result {};
    for (const auto& row : records) {
        result += std::count_if(row.begin(), row.end(), [](auto val) { return val != 0; });
    }

    std::cout << "Result part 1: " << result << std::endl;
    return 0;
}
