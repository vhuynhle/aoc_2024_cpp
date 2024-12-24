#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <map>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

enum class Operator {
    XOR = 0,
    AND = 1,
    OR = 2,
};

using LHS = std::tuple<std::string, std::string, Operator>;

Operator str2op(std::string_view s)
{
    if (s == "XOR") {
        return Operator::XOR;
    } else if (s == "AND") {
        return Operator::AND;
    } else if (s == "OR") {
        return Operator::OR;
    }
    throw std::invalid_argument(std::format("Not a valid operator string: {}", s));
}

std::string_view op2str(Operator op)
{
    switch (op) {
    case Operator::XOR:
        return "XOR";
    case Operator::AND:
        return "AND";
    case Operator::OR:
        return "OR";
    }
    throw std::invalid_argument("Not a valid operator");
}

std::string find_gate(
    const std::map<LHS, std::string>& gates, std::string in_1, std::string in_2, Operator op)
{
    if (in_1 > in_2) {
        std::swap(in_1, in_2);
    }

    auto it = gates.find(LHS(std::move(in_1), std::move(in_2), op));
    if (it != gates.end()) {
        return it->second;
    }
    return "";
}

void part_2(std::map<LHS, std::string>& ins_to_out, std::map<std::string, LHS>& out_to_ins)
{
    // Schematic:
    //
    // Adding first bits:
    // x -------- XOR ------- out_1
    //     |    |
    // y --|----+
    // |   |
    // |   +---------- AND -- ic1
    // |              |
    // +--------------+
    //
    // Adding other bits:
    // i > 0
    // Adding nth bits with carry from the previous round
    // Schematic:
    // carry -------------------- XOR-----------------------out_2[z]
    //                  |         |
    //                  +---------|----+ AND----- ic_2
    //                            |    |            |
    // x --------- XOR --- out_1--+----+            |
    //       |    |                                 |
    // y ----|----|                                 |
    //       |    |                                 |
    //       |--- AND ---- ic_1---------------------+ OR --- final_carry (to next round)

    std::vector<std::string> swapped {};
    std::string carry {};
    std::string out_1 {};
    std::string ic_1 {};
    std::string ic_2 {};
    std::string out_2 {};

    const auto refresh_pins = [&](auto xor_lhs, auto and_lhs) {
        out_1 = ins_to_out.at(xor_lhs);
        ic_1 = ins_to_out.at(and_lhs);
        out_2 = find_gate(ins_to_out, carry, out_1, Operator::XOR);
    };

    const auto swap_pins
        = [&ins_to_out, &out_to_ins, &swapped](const std::string& out1, const std::string& out2) {
              std::println(">>>>>>>> Switching {} <-> {}\n", out1, out2);
              auto lhs1 = out_to_ins.at(out1);
              auto lhs2 = out_to_ins.at(out2);

              out_to_ins[out1] = lhs2;
              out_to_ins[out2] = lhs1;

              ins_to_out[lhs1] = out2;
              ins_to_out[lhs2] = out1;

              swapped.push_back(out1);
              swapped.push_back(out2);
          };

    {
        // First round
        out_1 = ins_to_out.at(LHS { "x00", "y00", Operator::XOR });
        ic_1 = ins_to_out.at(LHS { "x00", "y00", Operator::AND });
        if (out_1 != "z00") {
            std::println("First output not match: actual {}, expected z00", out_1);
            throw std::runtime_error("Not handled error from the start");
        }
        carry = ic_1;
        std::println("Round 0 ok, carry = {}\n", carry);
    }

    for (std::size_t i { 1 }; i < 45; ++i) {
        const std::string x = std::format("x{:02}", i);
        const std::string y = std::format("y{:02}", i);
        const std::string z = std::format("z{:02}", i);

        const auto xor_lhs = LHS { x, y, Operator::XOR };
        out_1 = ins_to_out.at(xor_lhs);
        const auto and_lhs = LHS { x, y, Operator::AND };
        ic_1 = ins_to_out.at(and_lhs);
        std::println("Round {}: out_1 = {}, ic_1 = {}", i, out_1, ic_1);

        std::println("Expecting: {} XOR {} -> {}", carry, out_1, z);
        out_2 = find_gate(ins_to_out, carry, out_1, Operator::XOR);

        if (!out_2.empty()) {
            if (out_2 == z) {
                std::println("OK, out_2 = {}", out_2);
            } else {
                std::println(
                    "********************************************************************");
                std::println("* Output not match: {} XOR {} -> {} vs {}", carry, out_1, out_2, z);
                std::println(
                    "********************************************************************");
                swap_pins(out_2, z);
                refresh_pins(xor_lhs, and_lhs);
            }
        } else {
            std::println("********************************************************************");
            std::println("* No output found: {} XOR {} -> !", carry, out_1);
            std::println("********************************************************************");

            auto [other_in1, other_in2, other_op] = out_to_ins[z];
            std::println("Switch: {} {} {} -> {}", other_in1, op2str(other_op), other_in2, z);

            if (carry == other_in1) {
                swap_pins(out_1, other_in2);
            } else if (carry == other_in2) {
                swap_pins(out_1, other_in1);
            } else if (out_1 == other_in1) {
                swap_pins(carry, other_in2);
                carry = other_in2;
            } else if (out_1 == other_in2) {
                swap_pins(carry, other_in1);
                carry = other_in1;
            } else {
                std::println("Not correctable.");
                std::exit(EXIT_FAILURE);
            }

            refresh_pins(xor_lhs, and_lhs);
        }

        std::println("Expecting {} AND {} -> <ic_2>", carry, out_1);
        ic_2 = find_gate(ins_to_out, carry, out_1, Operator::AND);
        if (!ic_2.empty()) {
            std::println("OK, ic_2 = {}", ic_2);
        } else {
            std::println("ERROR, intermediate carry bit not found: {} AND {} -> !", carry, out_1);
            std::exit(EXIT_FAILURE);
        }

        std::println("Expecting {} OR {} -> <carry>", ic_1, ic_2);
        std::optional<std::string> final_carry = find_gate(ins_to_out, ic_1, ic_2, Operator::OR);
        if (final_carry) {
            std::println("OK, carry: {}", *final_carry);
            carry = *final_carry;
        } else {
            std::println("ERROR: Final carry line not found");
            std::exit(EXIT_FAILURE);
        }

        std::println();
    }

    std::println("Part 2 result:");
    std::sort(swapped.begin(), swapped.end());
    assert(swapped.size() > 0);
    std::print("{}", swapped.front());
    for (std::size_t i { 1 }; i < swapped.size(); ++i) {
        std::print(",{}", swapped[i]);
    }
    std::println();
}

int main()
{

    std::map<std::string, bool> line_values;

    std::string str;

    while (true) {
        std::getline(std::cin, str);
        if (str.empty()) {
            break;
        }

        const std::string line = str.substr(0, 3);
        const bool value = static_cast<bool>(std::strtoul(&str[5], nullptr, 10));

        line_values[line] = value;
    }

    std::string in_1;
    std::string in_2;
    std::string op_str;
    std::string ignore;
    std::string out;

    std::map<std::string, LHS> out_to_ins;
    std::map<LHS, std::string> ins_to_out;

    while ((std::cin >> in_1 >> op_str >> in_2 >> ignore >> out)) {
        if (in_1 > in_2) {
            std::swap(in_1, in_2);
        }
        Operator op = str2op(op_str);

        out_to_ins[out] = LHS { in_1, in_2, op };
        ins_to_out[LHS { in_1, in_2, op }] = out;
    }

    // Part 2
    part_2(ins_to_out, out_to_ins);
    return 0;
}
