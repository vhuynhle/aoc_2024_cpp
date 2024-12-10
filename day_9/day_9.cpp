#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

static constexpr std::uint64_t index_npos { std::numeric_limits<std::uint64_t>::max() };
static constexpr std::uint64_t free_block_marker { std::numeric_limits<std::uint64_t>::max() };

struct Block {
    std::uint64_t offset;
    std::uint64_t length;
};

static inline constexpr std::uint8_t digit_to_num(char digit)
{
    return static_cast<std::uint8_t>(digit - '0');
}

std::pair<std::vector<std::uint64_t>, std::list<Block>> decode_disk(std::string_view encoded_disk)
{
    std::vector<std::uint64_t> disk {};
    std::list<Block> free_blocks {};

    std::uint64_t i { 0 };
    for (i = 0; i < encoded_disk.length() / 2; ++i) {
        const std::uint64_t file_block_length = digit_to_num(encoded_disk[(i << 1) | 0]);
        disk.insert(disk.end(), file_block_length, i);

        const std::uint64_t free_block_length = digit_to_num(encoded_disk[(i << 1) | 1]);
        if (free_block_length > 0) {
            free_blocks.emplace_back(disk.size(), free_block_length);
            disk.insert(disk.end(), free_block_length, free_block_marker);
        }
    }

    if (encoded_disk.length() % 2 == 1) {
        const auto file_block_length = digit_to_num(encoded_disk[(i << 1) | 0]);
        disk.insert(disk.end(), file_block_length, i);
    }

    return { disk, free_blocks };
}

void compact(std::span<std::uint64_t> disk)
{
    const auto find_free_cell = [&disk](std::uint64_t pos) {
        while (pos < disk.size() && disk[pos] != free_block_marker) {
            ++pos;
        }
        return pos;
    };

    const auto find_file_cell = [&disk](std::uint64_t pos) {
        while (pos != index_npos && disk[pos] == free_block_marker) {
            --pos;
        }
        return pos;
    };

    std::uint64_t i { find_free_cell(0) };
    std::uint64_t j { find_file_cell(disk.size() - 1) };
    while ((i < j) && (j != index_npos)) {
        std::swap(disk[i], disk[j]);
        i = find_free_cell(i + 1);
        j = find_file_cell(j - 1);
    }
}

std::uint64_t checksum(std::span<std::uint64_t> disk)
{
    std::uint64_t sum { 0 };
    for (std::uint64_t i { 0 }; i < disk.size(); ++i) {
        if (disk[i] != free_block_marker) {
            sum += i * disk[i];
        }
    }

    return sum;
}

Block find_disk_block(std::span<std::uint64_t> disk, std::uint64_t end_pos)
{
    while (end_pos != index_npos && disk[end_pos] == free_block_marker) {
        --end_pos;
    }

    if (end_pos == index_npos) {
        return Block { index_npos, 0 };
    }

    std::uint64_t start_pos { end_pos };
    while (start_pos != index_npos && disk[start_pos] == disk[end_pos]) {
        --start_pos;
    }
    ++start_pos;

    return Block { start_pos, end_pos - start_pos + 1 };
};

auto find_free_block(
    std::list<Block>& free_blocks, std::uint64_t end_offset, std::uint64_t min_length)
{
    auto free_block_it = free_blocks.begin();
    bool free_block_found = false;
    while (free_block_it != free_blocks.end() && free_block_it->offset < end_offset) {
        if (free_block_it->length >= min_length) {
            free_block_found = true;
            break;
        }
        ++free_block_it;
    }

    return free_block_found ? free_block_it : free_blocks.end();
};

void defrag(std::span<std::uint64_t> disk, std::list<Block>& free_blocks)
{
    Block file_block = find_disk_block(disk, disk.size() - 1);
    while (file_block.offset != index_npos) {
        auto free_block_it = find_free_block(free_blocks, file_block.offset, file_block.length);
        if (free_block_it != free_blocks.end()) {
            // Update the disk
            std::swap_ranges(disk.begin() + static_cast<std::ptrdiff_t>(free_block_it->offset),
                disk.begin()
                    + static_cast<std::ptrdiff_t>(free_block_it->offset + file_block.length),
                disk.begin() + static_cast<std::ptrdiff_t>(file_block.offset));

            // Update the free list
            // free_block_it shrinks
            free_block_it->offset += file_block.length;
            free_block_it->length -= file_block.length;
            if (free_block_it->length == 0) {
                free_blocks.erase(free_block_it);
            }
            // The last free block grows. Here we can grow the last block and merge it with
            // its preceding free block if they are next to each other.
            // However we don't need to correct it, because we don't need it again.
        }

        file_block = find_disk_block(disk, file_block.offset - 1);
    }
}

int main()
{
    std::string encoded_disk;
    std::getline(std::cin, encoded_disk);

    auto [disk, free_blocks] = decode_disk(encoded_disk);
    auto disk_copy = disk;
    compact(disk);
    std::cout << "Part 1 result: " << checksum(disk) << std::endl;

    defrag(disk_copy, free_blocks);
    std::cout << "Part 2 result: " << checksum(disk_copy) << std::endl;

    return 0;
}
