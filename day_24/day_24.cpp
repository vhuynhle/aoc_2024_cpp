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

static std::uint64_t evaluate(const std::map<std::string, LHS>& gates, const std::string& out,
    std::map<std::string, bool>& line_values)
{
    auto it = line_values.find(out);
    if (it != line_values.end()) {
        return it->second;
    }

    const auto& [in_1, in_2, op] = gates.at(out);
    const auto in_1_val = evaluate(gates, in_1, line_values);
    const auto in_2_val = evaluate(gates, in_2, line_values);

    bool out_value {};
    switch (op) {
    case Operator::XOR:
        out_value = in_1_val != in_2_val;
        break;
    case Operator::AND:
        out_value = in_1_val && in_2_val;
        break;
    case Operator::OR:
        out_value = in_1_val || in_2_val;
        break;
    }

    line_values[out] = out_value;
    return out_value;
}

const std::string* find_gate(
    const std::map<LHS, std::string>& gates, std::string in_1, std::string in_2, Operator op)
{
    if (in_1 > in_2) {
        std::swap(in_1, in_2);
    }

    auto it = gates.find(LHS(std::move(in_1), std::move(in_2), op));
    if (it != gates.end()) {
        return &it->second;
    }
    return nullptr;
}

void part_1(const std::map<std::string, LHS>& out_to_ins, std::map<std::string, bool> line_values)
{
    for (const auto& gate : out_to_ins) {
        evaluate(out_to_ins, gate.first, line_values);
    }

    std::uint64_t z_values {};
    std::uint64_t z_bit {};
    for (auto& [k, v] : line_values) {
        if (k[0] == 'z') {
            z_values |= static_cast<std::uint64_t>(v) << z_bit;
            ++z_bit;
        }
    }

    std::println("Part 1 result: {}", z_values);
}

void part_2(
    std::vector<std::pair<LHS, std::string>>& xy_gates, std::map<LHS, std::string>& ins_to_out)
{
    std::string carry {};
    for (std::size_t i { 0 }; i < xy_gates.size(); i += 2) {
        const auto& [xor_lhs, out_1] = xy_gates[i];
        const auto& [and_lhs, ic_2] = xy_gates[i + 1];

        // Validate out assumption about the input
        const std::string x_in = std::format("x{:02}", i / 2);
        const std::string y_in = std::format("y{:02}", i / 2);
        const std::string z_out = std::format("z{:02}", i / 2);

        if (xor_lhs != LHS { x_in, y_in, Operator::XOR }) {
            throw std::runtime_error("Wrong assumption about xy XOR gate");
        }
        if (and_lhs != LHS { x_in, y_in, Operator::AND }) {
            throw std::runtime_error("Wrong assumption about xy AND gate");
        }

        if (i == 0) {
            // Adding the first bits
            // Schematic:
            // x00 -------- XOR --- z00
            //       |    |
            // y00 --|----+
            //    |  |
            //    |  +----- AND -- carry
            //    +--------+
            const auto& [xor_lhs, actual_out] = xy_gates[0];
            const auto& [and_lhs, expected_carry] = xy_gates[1];

            if (actual_out != z_out) {
                std::println("First output not match: actual {}, expected {}", actual_out, z_out);
                std::exit(EXIT_FAILURE);
            }
            carry = expected_carry;

            std::println("Round 0 ok, carry = {}\n", carry);
            continue;
        }

        // i > 0
        // Adding nth bits with carry from the previous round
        // Schematic:
        // carry -------------------- XOR----------------------- z_out
        //                  |         |
        //                  +---------|----+ AND----- ic_1
        //                            |    |            |
        // x --------- XOR --- out_1--+----+            |
        //       |    |                                 |
        // y ----|----|                                 |
        //       |    |                                 |
        //       |--- AND ---- ic_2---------------------+ OR --- final_carry (to next round)
        std::println("Expecting: {} XOR {} -> {}", carry, out_1, z_out);
        const std::string* actual_out = find_gate(ins_to_out, carry, out_1, Operator::XOR);
        if (actual_out) {
            if (*actual_out == z_out) {
                std::println("OK");
            } else {
                std::println(
                    "Output not match: {} XOR {} -> {} vs {}", carry, out_1, *actual_out, z_out);
                std::exit(EXIT_FAILURE);
            }
        } else {
            std::println("No output found: {} XOR {} -> !", carry, out_1);
            std::exit(EXIT_FAILURE);
        }

        std::println("Expecting {} AND {} -> <intermediate carry bit 1>", carry, out_1);
        const std::string* ic_1 = find_gate(ins_to_out, carry, out_1, Operator::AND);
        if (ic_1) {
            std::println("OK, ic_1 = {}", *ic_1);
        } else {
            std::println("ERROR, intermediate carry bit not found: {} AND {} -> !", carry, out_1);
            std::exit(EXIT_FAILURE);
        }

        std::println("Expecting {} OR {} -> <carry>", *ic_1, ic_2);
        const std::string* final_carry = find_gate(ins_to_out, *ic_1, ic_2, Operator::OR);
        if (final_carry) {
            std::println("OK, carry: {}", *final_carry);
            carry = *final_carry;
        } else {
            std::println("ERROR: Final carry line not found");
            std::exit(EXIT_FAILURE);
        }

        std::println();
    }
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
    std::vector<std::pair<LHS, std::string>> xy_gates;

    while ((std::cin >> in_1 >> op_str >> in_2 >> ignore >> out)) {
        if (in_1 > in_2) {
            std::swap(in_1, in_2);
        }
        Operator op = str2op(op_str);

        out_to_ins[out] = LHS { in_1, in_2, op };
        ins_to_out[LHS { in_1, in_2, op }] = out;

        if (in_1[0] == 'x') {
            xy_gates.push_back(std::make_pair(LHS { in_1, in_2, op }, out));
        }
    }
    std::sort(xy_gates.begin(), xy_gates.end());

    part_1(out_to_ins, line_values);

    // Part 2
    part_2(xy_gates, ins_to_out);
    return 0;
}
