#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

using Position = std::pair<std::uint64_t, std::uint64_t>;
using PathCache = std::map<std::pair<char, char>, std::vector<std::string>>;

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

static std::uint64_t get_complexity_p1(const std::string& keycode);

static std::uint64_t get_complexity(const std::string& keycode, std::uint64_t max_level);

// The cost to move from a key to another at a given level
static std::map<std::tuple<char, char, std::uint64_t>, std::uint64_t> cost_cache;

int main()
{
    std::uint64_t complexity_p1_sum {};
    std::uint64_t complexity_p2_sum {};
    std::string line;

    while (std::getline(std::cin, line)) {
        const auto complexity_p1 = get_complexity_p1(line);
        const auto complexity_p2 = get_complexity(line, 25);
        complexity_p1_sum += complexity_p1;
        complexity_p2_sum += complexity_p2;
        std::println("{} -> {} {}", line, complexity_p1, complexity_p2);
    }

    std::println("Part 1 result: {}", complexity_p1_sum);
    std::println("Part 2 result: {}", complexity_p2_sum);

    return 0;
}

/** Find the path from a starting postion to an end position on a keypad */
static std::vector<std::string> find_path(
    std::span<const std::string_view> keypad, Position start, Position end)
{
    if (start == end) {
        return { "A" };
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

/** Find the paths between all pairs of positions on a keypad */
static PathCache find_all_paths(std::span<const std::string_view> keypad)
{
    PathCache cache;

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

/** All the path on the numeric keypad */
static const auto nk_paths = find_all_paths(numerical_keypad);

/** All the path on the directional keypad */
static const auto dk_paths = find_all_paths(directional_keypad);

/** Find all possible movement sequences to enter a key sequence
 *
 * @param paths
 *        Cache indicating how to move from one key to another on this keyboard
 * @param start
 *        The initial position on this keyboard
 * @param keycode
 *        The desired output of the input sequences to find
 */
static std::vector<std::string> find_movements(
    const PathCache& paths, char start, std::string_view sequence)
{
    const char current_key = sequence.front();
    const auto& moves_for_current_key = paths.at(std::make_pair(start, current_key));
    if (sequence.length() == 1) {
        return moves_for_current_key;
    }

    const auto moves_for_rest = find_movements(paths, current_key, sequence.substr(1));

    std::vector<std::string> res;
    for (const auto& move_for_current : moves_for_current_key) {
        for (const auto& move_for_rest : moves_for_rest) {
            res.push_back(move_for_current + move_for_rest);
        }
    }

    return res;
}

/** Find all possible movement sequences to input one of the given sequences
 *
 * @param paths
 *        Cache indicating how to move from one key to another on this keyboard
 * @param start
 *        The initial position on this keyboard
 * @param keycode
 *        The desired output of the input sequences to find
 */
static std::vector<std::string> find_movements(
    const PathCache& cache, char start, std::span<const std::string> sequences)
{
    std::vector<std::string> res;

    for (const auto& sequence : sequences) {
        auto movements = find_movements(cache, start, sequence);

        for (auto& moves : movements) {
            res.emplace_back(std::move(moves));
        }
    }

    return res;
}

static std::uint64_t get_cost_p1(char from, char to)
{
    // Movements from start to end on the numeric keypad
    auto level_0_movements = nk_paths.at(std::make_pair(from, to));

    // Movements on the first directional keypad
    auto level_1_movements = find_movements(dk_paths, 'A', level_0_movements);

    // Movements on the second directional keypad
    auto level_2_movements = find_movements(dk_paths, 'A', level_1_movements);

    // To produce movements on the second directional keyboard,
    // the human operator types *directly* on the third directional keyboard.
    // So the amount of typing is the same as the lengths of the movements on the second directional
    // keypad

    // Choose the shortest sequence
    auto min_it = std::min_element(level_2_movements.begin(), level_2_movements.end(),
        [](const auto& s1, const auto& s2) { return s1.size() < s2.size(); });
    return min_it->length();
}

/** Find the cost to move from 1 character to another on the numerical keypad */
static std::uint64_t get_cost_p1(std::string_view keycode)
{
    // The cost of the whole keycode can be partitioned into the costs
    // of moving from one character to another:
    // Because after pressing on each character, the position on the upper layer keypad
    // is reset to 'A' -> the cost of moving from pos1 -> pos2 and from pos2 -> pos3 are independent.

    std::uint64_t cost {};
    char prev = 'A'; // initial position
    for (char ch : keycode) {
        cost += get_cost_p1(prev, ch);
        prev = ch;
    }
    return cost;
}

static std::uint64_t get_complexity_p1(const std::string& keycode)
{
    return std::stoull(keycode) * get_cost_p1(keycode);
}

// The cost to move from one character to another on a level
static std::uint64_t get_cost(char from, char to, std::uint64_t level, std::uint64_t max_level);

static std::uint64_t get_cost_str(
    const std::string& code, std::uint64_t level, std::uint64_t max_level)
{
    std::uint64_t cost { 0 };
    char prev_key { 'A' };
    for (auto ch : code) {
        cost += get_cost(prev_key, ch, level, max_level);
        prev_key = ch;
    }

    return cost;
}

static std::uint64_t get_cost(char from, char to, std::uint64_t level, std::uint64_t max_level)
{
    auto& res = cost_cache[std::make_tuple(from, to, level)];
    if (res != 0) {
        return res;
    }

    // All the path from `from` to `to`
    const auto& paths = [from, to, level]() -> std::vector<std::string> {
        auto& path_cache = (level == 0) ? nk_paths : dk_paths;
        return path_cache.at(std::make_pair(from, to));
    }();

    // Outer most level: direct cost.
    if (level == max_level) {
        res = paths.front().size();
        return res;
    }

    std::uint64_t min_cost { std::numeric_limits<std::uint64_t>::max() };
    for (const auto& path : paths) {
        min_cost = std::min(min_cost, get_cost_str(path, level + 1, max_level));
    }

    res = min_cost;
    return res;
}

static std::uint64_t get_complexity(const std::string& keycode, std::uint64_t max_level)
{
    return std::stoull(keycode) * get_cost_str(keycode, 0, max_level);
}
