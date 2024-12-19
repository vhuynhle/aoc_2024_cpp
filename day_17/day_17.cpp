#include <algorithm>
#include <cstdint>
#include <format>
#include <iostream>
#include <iterator>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Chip {
public:
    Chip(std::uint64_t a, std::uint64_t b, std::uint64_t c, std::vector<std::uint8_t> mem)
        : pc {}
        , regA { a }
        , regB { b }
        , regC { c }
        , memory { std::move(mem) }
    {
        if (memory.size() < 2) {
            throw std::runtime_error { "Program too short" };
        }
    }

    std::uint64_t pc;
    std::uint64_t regA;
    std::uint64_t regB;
    std::uint64_t regC;
    std::vector<std::uint8_t> memory;

    void execute()
    {
        bool first { true };
        while (!halted()) {
            const std::optional<std::uint8_t> output = step();
            if (output.has_value()) {
                if (first) {
                    std::print("{}", *output);
                    first = false;
                } else {
                    std::print(",{}", *output);
                }
            }
        }
    }

    bool halted() const
    {
        return pc + 1 >= memory.size();
    }

    std::optional<std::uint8_t> step()
    {
        const std::uint64_t opcode { memory[pc] };
        const std::uint64_t literal_op { memory[pc + 1] };
        const std::uint64_t combo_op = to_combo_op(literal_op);
        pc += 2;

        switch (opcode) {
        case 0: // adv
            regA >>= combo_op;
            break;
        case 1: // bxl
            regB ^= literal_op;
            break;
        case 2: // bst
            regB = (combo_op & 0b111ULL);
            break;
        case 3: // jnz
            if (regA) {
                pc = literal_op;
            }
            break;
        case 4:
            regB ^= regC;
            break;
        case 5: // out
            return static_cast<std::uint8_t>(combo_op & 0b111ULL);
        case 6: // bdv
            regB = regA >> combo_op;
            break;
        case 7: // cdv
            regC = regA >> combo_op;
            break;
        }

        return std::nullopt;
    }

    std::uint64_t to_combo_op(std::uint64_t operand)
    {
        switch (operand) {
        case 0:
        case 1:
        case 2:
        case 3:
            return operand;

        case 4:
            return regA;
        case 5:
            return regB;
        case 6:
            return regC;
        default:
            std::string message;
            std::format_to(std::back_inserter(message), "Invalid operand: {:d}", operand);
            throw std::runtime_error(message);
        }
    }
};

namespace p2 {
std::pair<std::uint64_t, std::uint8_t> process(std::uint64_t a)
{
    if (a == 0) {
        return { 0, 255u };
    }

    auto b = (a & 0b111) ^ 3;
    auto c = (a >> b);
    b ^= 5;
    a >>= 3;
    b ^= c;
    const auto out = static_cast<std::uint8_t>(b & 0b111);
    return { a, out };
}

std::vector<std::uint64_t> reg_a_preimage(std::uint64_t a, std::uint8_t out)
{
    std::vector<std::uint64_t> preimages {};
    for (std::uint64_t preimage = a * 8; preimage < a * 8 + 8; ++preimage) {
        if (process(preimage) == std::make_pair(a, out)) {
            preimages.push_back(preimage);
        }
    }

    return preimages;
}

/// Find the pre-image values of register A so that
// p2::process(preimage) set reg A to reg_a_out and return the desired output
std::vector<std::uint64_t> traceback(
    std::vector<std::uint64_t> reg_a_out, std::span<const std::uint8_t> reversed_output)
{
    if (reversed_output.empty()) {
        return reg_a_out;
    }

    std::vector<std::uint64_t> res;
    for (auto reg_a_after : reg_a_out) {
        auto reg_a_preimages = reg_a_preimage(reg_a_after, reversed_output.front());
        std::copy(reg_a_preimages.begin(), reg_a_preimages.end(), std::back_inserter(res));
    }

    return traceback(res, reversed_output.subspan(1));
}

}

int main()
{
    std::string line;
    std::string ignore;

    std::uint64_t initA;
    std::uint64_t initB;
    std::uint64_t initC;

    std::cin >> ignore;
    std::cin >> ignore;
    std::cin >> initA;

    std::cin >> ignore;
    std::cin >> ignore;
    std::cin >> initB;

    std::cin >> ignore;
    std::cin >> ignore;
    std::cin >> initC;

    std::cin >> ignore;

    std::vector<std::uint8_t> memory;
    std::uint16_t num { 7 };
    char ignore_char;
    while (true) {
        std::cin >> num;
        memory.push_back(static_cast<std::uint8_t>(num));
        if (!(std::cin >> ignore_char)) {
            break;
        }
    }
    std::println();

    // Part 1
    Chip chip { initA, initB, initC, memory };
    chip.execute();

    std::reverse(memory.begin(), memory.end());

    const auto candidates = p2::traceback({ 0 }, memory);
    auto res = std::min_element(candidates.begin(), candidates.end());
    std::println("Part 2 result: {}", *res);

    return 0;
}
