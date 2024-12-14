#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <format>
#include <fstream>
#include <iterator>
#include <numeric>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

struct Config {
    std::int64_t x;
    std::int64_t y;
    std::int64_t vx;
    std::int64_t vy;
};

std::optional<Config> parse_line(std::string_view line);

std::pair<std::int64_t, std::int64_t> move(
    const Config& config, std::int64_t width, std::int64_t height, std::int64_t moves);

inline std::optional<std::uint64_t> get_quadrant(
    std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height);

void step(std::span<Config> current_configs, std::int64_t width, std::int64_t height);

void to_picture(std::span<const Config> configs, std::span<std::byte> pic, std::int64_t width);

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::println("Usage: {} <width> <height> <configuration file>", argv[0]);
        return EXIT_FAILURE;
    }
    constexpr std::int64_t num_moves { 100 };

    const std::int64_t width { std::strtoll(argv[1], nullptr, 10) };
    const std::int64_t height { std::stoi(argv[2], nullptr, 10) };
    std::ifstream ifs { argv[3] };

    std::vector<Config> configs;
    std::string line;
    while (std::getline(ifs, line)) {
        const auto config = parse_line(line).value();
        if (line.empty()) {
            break;
        }
        configs.push_back(config);
    }

    // Part 1
    std::array<std::int64_t, 4> quadrants = { 0, 0, 0, 0 };
    for (const auto& config : configs) {
        auto [final_x, final_y] = move(config, width, height, num_moves);
        const auto maybe_quad = get_quadrant(final_x, final_y, width, height);
        if (maybe_quad.has_value()) {
            ++quadrants[maybe_quad.value()];
        }
    }
    std::int64_t part1_res = std::accumulate(quadrants.begin(), quadrants.end(),
        static_cast<std::int64_t>(1), [](auto acc, auto val) { return acc * val; });
    std::println("Part 1 result: {}", part1_res);

    // Part 2
    static constexpr std::int64_t rgb_pixel_bytes { 3 };
    std::vector<std::byte> picture(
        static_cast<std::uint64_t>(rgb_pixel_bytes * width * height), std::byte { 0 });

    std::string out_file {};
    for (std::size_t i {}; i < 10000; ++i) {
        to_picture(configs, picture, width);

        out_file.clear();
        std::format_to(std::back_inserter(out_file), "image_{:04d}.bmp", i);

        stbi_write_bmp(out_file.c_str(), static_cast<int>(width), static_cast<int>(height),
            rgb_pixel_bytes, picture.data());

        step(configs, width, height);
    }

    // Use an external tool to inspect the output images, e.g., Dolphin file explorer.
    // Viewing the images in "Compact" mode is best.
    // Result: 7383

    return 0;
}

std::optional<Config> parse_line(std::string_view line)
{
    std::int64_t x;
    const auto equal_pos_1 = line.find('=', 0);
    if (equal_pos_1 == std::string_view::npos) {
        return std::nullopt;
    }
    const auto comma_pos_1 = line.find(',', equal_pos_1 + 1);
    if (comma_pos_1 == std::string_view::npos) {
        return std::nullopt;
    }
    auto ret = std::from_chars(&line[equal_pos_1 + 1], &line[comma_pos_1], x);
    if ((ret.ec != std::errc {}) || (ret.ptr != &line[comma_pos_1])) {
        return std::nullopt;
    }

    std::int64_t y;
    const auto space_pos = line.find(' ', comma_pos_1 + 1);
    if (space_pos == std::string_view::npos) {
        return std::nullopt;
    }
    ret = std::from_chars(&line[comma_pos_1 + 1], &line[space_pos], y);
    if ((ret.ec != std::errc {}) || (ret.ptr != &line[space_pos])) {
        return std::nullopt;
    };

    std::int64_t vx;
    const auto equal_pos_2 = line.find('=', space_pos + 1);
    if (equal_pos_2 == std::string_view::npos) {
        return std::nullopt;
    }
    const auto comma_pos_2 = line.find(',', equal_pos_2 + 1);
    if (comma_pos_2 == std::string_view::npos) {
        return std::nullopt;
    }
    ret = std::from_chars(&line[equal_pos_2 + 1], &line[comma_pos_2], vx);
    if ((ret.ec != std::errc {}) || (ret.ptr != &line[comma_pos_2])) {
        return std::nullopt;
    };

    std::int64_t vy;
    ret = std::from_chars(&line[comma_pos_2 + 1], line.data() + line.length(), vy);
    if ((ret.ec != std::errc {}) || (ret.ptr != line.data() + line.length())) {
        return std::nullopt;
    }

    return Config { x, y, vx, vy };
}

std::pair<std::int64_t, std::int64_t> move(
    const Config& config, std::int64_t width, std::int64_t height, std::int64_t moves)
{
    const auto mod = [](auto x, auto y) { return ((x % y) + y) % y; };

    const std::int64_t final_x = mod(config.x + config.vx * moves, width);
    const std::int64_t final_y = mod(config.y + config.vy * moves, height);
    return { final_x, final_y };
}

inline std::optional<uint64_t> get_quadrant(
    std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height)
{
    if (x == (width >> 1) || y == (height >> 1)) {
        return std::nullopt;
    }

    const auto px = static_cast<std::uint64_t>(x >= (width >> 1));
    const auto py = static_cast<std::uint64_t>(y >= (height >> 1));
    return (px << 1) | py;
}

void step(std::span<Config> configs, std::int64_t width, std::int64_t height)
{
    for (auto& config : configs) {
        const auto [next_x, next_y] = move(config, width, height, 1);
        config.x = next_x;
        config.y = next_y;
    }
}

void to_picture(std::span<const Config> configs, std::span<std::byte> pic, std::int64_t width)
{
    std::fill(pic.begin(), pic.end(), std::byte { 255 });
    for (const auto& config : configs) {
        std::uint64_t pixel = static_cast<std::size_t>(config.y * width + config.x);
        pic[3 * pixel + 0] = std::byte { 0 };
        pic[3 * pixel + 1] = std::byte { 0 };
        pic[3 * pixel + 2] = std::byte { 0 };
    }
}
