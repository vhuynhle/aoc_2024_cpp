#include <algorithm>
#include <cstdint>
#include <deque>
#include <format>
#include <iostream>
#include <iterator>
#include <optional>
#include <print>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
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
            throw std::runtime_error { "Program too short." };
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

            std::println("{} {} {} {}", regA, regB, regC, pc);

            const std::optional<std::uint8_t> output = step();
            if (output.has_value()) {
                if (first) {
                    std::println("{}", *output);
                    first = false;
                } else {
                    std::println("{}", *output);
                }
            }
        }

        std::println("\n>>> Halted <<<");
    }

    bool check_self_generation()
    {
        std::vector<std::uint8_t> output_sequence {};

        while (!halted()) {
            auto output = step();
            if (output) {
                output_sequence.emplace_back(*output);
                if (output_sequence.size() == memory.size()) {
                    return output_sequence == memory;
                }
            }
        }

        return false;
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
        std::print("{} ", num);
        if (!(std::cin >> ignore_char)) {
            break;
        }
    }
    std::println();

    // Part 1
    Chip chip { initA, initB, initC, std::move(memory) };
    chip.execute();


    // Part 2
    // initA = 0;
    // while (true) {
    //     chip.regA = initA;
    //     chip.regB = initB;
    //     chip.regC = initC;
    //     chip.pc = 0;
    //     if (chip.check_self_generation()) {
    //         break;
    //     }

    //     ++initA;
    // }

    // std::println("Part 2 result: {}", initA);

    return 0;
}
