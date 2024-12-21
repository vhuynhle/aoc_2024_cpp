#include <array>
#include <cstdint>
#include <map>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using Position = std::pair<std::uint64_t, std::uint64_t>;

static constexpr std::array<std::string_view, 4> directional_keypad = {
    "#####",
    "##^A#",
    "#<v>#",
    "#####",
};

static constexpr std::array<std::string_view, 6> numerical_keypad = {
    "#####",
    "#789#",
    "#456#",
    "#123#",
    "##0A#",
    "#####",
};

std::vector<std::string> find_path(
    std::span<const std::string_view> keypad, Position start, Position end)
{
    if (start == end) {
        return { "" };
    }

    std::vector<std::string> res {};
    if (start.first != end.first) {
        const auto next_row = (start.first < end.first) ? (start.first + 1) : (start.first - 1);
        const std::string move = (start.first < end.first) ? "v" : "^";
        if (keypad[next_row][start.second] != '#') {
            auto partial_res = find_path(keypad, Position { next_row, start.second }, end);
            for (const auto& partial_path : partial_res) {
                res.emplace_back(move + partial_path);
            }
        }
    }

    if (start.second != end.second) {
        const auto next_col = (start.second < end.second) ? (start.second + 1) : (start.second - 1);
        const std::string move = (start.second < end.second) ? ">" : "<";
        if (keypad[start.first][next_col] != '#') {
            auto partial_res = find_path(keypad, Position { start.first, next_col }, end);
            for (const auto& partial_path : partial_res) {
                res.emplace_back(move + partial_path);
            }
        }
    }

    return res;
}

std::map<std::pair<char, char>, std::vector<std::string>> make_path_cache(
    std::span<const std::string_view> keypad)
{
    std::map<std::pair<char, char>, std::vector<std::string>> cache;

    for (std::uint64_t start_r { 1 }; start_r < keypad.size() - 1; ++start_r) {
        for (std::uint64_t start_c { 1 }; start_c < keypad.front().size() - 1; ++start_c) {
            if (keypad[start_r][start_c] == '#') {
                continue;
            }

            for (std::uint64_t end_r { 1 }; end_r < keypad.size() - 1; ++end_r) {
                for (std::uint64_t end_c { 1 }; end_c < keypad.front().size() - 1; ++end_c) {
                    if (keypad[end_r][end_c] == '#') {
                        continue;
                    }

                    cache[std::make_pair(keypad[start_r][start_c], keypad[end_r][end_c])]
                        = find_path(
                            keypad, Position { start_r, start_c }, Position { end_r, end_c });
                }
            }
        }
    }
    return cache;
}

int main()
{
    make_path_cache(directional_keypad);
    make_path_cache(numerical_keypad);
    return 0;
}
