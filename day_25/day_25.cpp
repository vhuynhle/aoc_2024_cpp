#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <ostream>
#include <print>
#include <string>
#include <utility>
#include <vector>

static constexpr std::size_t kGridHeight { 7U };
static constexpr std::size_t kNumPins { 5 };

using PinHeights = std::array<std::uint8_t, kNumPins>;
using Grid = std::vector<std::string>;
using KeySet = std::vector<PinHeights>;
using LockSet = std::vector<PinHeights>;

std::array<std::uint8_t, kNumPins> get_pins(const Grid& grid)
{
    const std::size_t nrows { grid.size() };
    const std::size_t ncols { grid.front().size() };
    std::array<std::uint8_t, kNumPins> pin_heights {};
    assert(nrows == kGridHeight);
    assert(ncols == kNumPins);

    for (std::size_t col { 0 }; col < ncols; ++col) {
        std::size_t row;
        for (row = 0; row < nrows && grid[row][col] == grid[0][0]; ++row) { }
        pin_heights[col] = static_cast<std::uint8_t>(row);
    }

    return pin_heights;
}

std::pair<LockSet, KeySet> read_input(std::istream& stream)
{
    std::string line;
    bool eof { false };

    LockSet locks;
    KeySet keys;

    while (!eof) {
        bool block_done { false };
        Grid grid;

        while (!block_done) {
            std::getline(stream, line);
            if (!stream) {
                eof = true;
                block_done = true;
            } else if (line.empty()) {
                block_done = true;
            } else {
                if (line.size() != kNumPins) {
                    std::println(std::cerr, "Invalid input: wrong number of pins");
                    ::exit(EXIT_FAILURE);
                }
                grid.push_back(line);
            }
        }

        if (grid.size() != kGridHeight) {
            std::println(std::cerr, "Invalid input: Wrong key/lock size");
            ::exit(EXIT_FAILURE);
        }

        if (grid[0][0] == '#') {
            locks.emplace_back(get_pins(grid));
        } else {
            keys.emplace_back(get_pins(grid));
        }
    }

    return { locks, keys };
}

bool fit(
    const std::array<std::uint8_t, kNumPins>& lock, const std::array<std::uint8_t, kNumPins>& key)
{
    for (std::size_t i { 0 }; i < kNumPins; ++i) {
        if (lock[i] > key[i]) {
            return false;
        }
    }
    return true;
}

void part_1(const LockSet& locks, const KeySet& keys)
{
    std::size_t fit_num {};
    for (const auto& lock : locks) {
        for (const auto& key : keys) {
            if (fit(lock, key)) {
                ++fit_num;
            }
        }
    }

    std::println("Part 1 result: {}", fit_num);
}

int main()
{
    const auto [locks, keys] = read_input(std::cin);
    std::println("Locks: {}", locks.size());
    std::println("Keys: {}", keys.size());

    part_1(locks, keys);

    return 0;
}
