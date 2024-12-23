#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <print>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

static constexpr std::uint16_t alphabet_size { static_cast<std::uint16_t>('z' - 'a' + 1) };
static constexpr std::uint16_t max_node_nums { alphabet_size * alphabet_size };

static inline std::uint16_t node_name_to_number(char ch1, char ch2)
{
    const auto c1_ord = static_cast<std::uint16_t>(ch1 - 'a');
    const auto c2_ord = static_cast<std::uint16_t>(ch2 - 'a');

    return static_cast<std::uint16_t>(c1_ord * alphabet_size + c2_ord);
}

static inline std::pair<char, char> number_to_node_name(std::uint16_t index)
{
    const auto c1 = static_cast<char>('a' + index / alphabet_size);
    const auto c2 = static_cast<char>('a' + index % alphabet_size);
    return { c1, c2 };
}

static inline bool add_if_not_exists(std::vector<std::uint16_t>& vec, std::uint16_t val)
{
    if (!std::ranges::contains(vec, val)) {
        vec.push_back(val);
        return true;
    }
    return false;
}

static inline std::tuple<std::uint16_t, std::uint16_t, std::uint16_t> sort_tuple(
    std::uint16_t a, std::uint16_t b, std::uint16_t c)
{
    const auto min_val = std::min(std::min(a, b), c);
    const auto max_val = std::max(std::max(a, b), c);
    const auto mid = static_cast<std::uint16_t>(a + b + c - min_val - max_val);
    return { min_val, mid, max_val };
}

static std::uint64_t find_max_kgraph_count;

/// Find the largest k-graph
/// The nodes returned are in reverse order
static std::vector<std::uint16_t> find_max_kgraph(std::span<const std::uint16_t> candidates,
    const std::vector<std::vector<std::uint16_t>>& adjacency_lists)
{
    ++find_max_kgraph_count;

    if (candidates.size() <= 1) {
        return { candidates.begin(), candidates.end() };
    }

    const auto n1 = candidates[0];
    if (candidates.size() == 2) {
        const auto n2 = candidates[1];
        if (std::ranges::contains(adjacency_lists[n1], n2)) {
            return std::vector(candidates.rbegin(), candidates.rend());
        } else {
            return { candidates[0] }; // {candidates[1]} is also fine
        }
    }

    // Type 1: complete graph containing the first node
    // This k-graph's vertices must be a subset of the adjacent vertices of the first node.
    std::vector<std::uint16_t> adjacent_candidates {};
    std::ranges::set_intersection(
        candidates, adjacency_lists[n1], std::back_inserter(adjacent_candidates));
    auto type1_kgraph = find_max_kgraph(adjacent_candidates, adjacency_lists);
    type1_kgraph.push_back(n1);

    // Type 2: complete graph *not* containing the first node
    auto type2_kgraph = find_max_kgraph(candidates.subspan(1), adjacency_lists);

    if (type1_kgraph.size() >= type2_kgraph.size()) {
        return type1_kgraph;
    } else {
        return type2_kgraph;
    }
}

static std::string nodes_to_str(std::span<const std::uint16_t> nodes)
{
    if (nodes.empty()) {
        return "";
    }

    std::string res;
    res.reserve(nodes.size() * 3);

    const auto [c1, c2] = number_to_node_name(nodes[0]);
    res.push_back(c1);
    res.push_back(c2);

    for (std::uint64_t i { 1 }; i < nodes.size(); ++i) {
        const auto [c1, c2] = number_to_node_name(nodes[i]);
        res.push_back(',');
        res.push_back(c1);
        res.push_back(c2);
    }

    return res;
}

int main()
{
    std::vector<std::vector<std::uint16_t>> adjacency_lists(
        max_node_nums, std::vector<std::uint16_t> {});

    std::vector<std::vector<std::uint16_t>> directed_adjacency_lists(
        max_node_nums, std::vector<std::uint16_t> {});

    std::string line;
    while (std::getline(std::cin, line)) {
        const auto pc1 = node_name_to_number(line[0], line[1]);
        const auto pc2 = node_name_to_number(line[3], line[4]);
        add_if_not_exists(adjacency_lists[pc1], pc2);
        add_if_not_exists(adjacency_lists[pc2], pc1);

        auto n1 = std::min(pc1, pc2);
        auto n2 = std::max(pc1, pc2);
        add_if_not_exists(directed_adjacency_lists[n1], n2);
    }

    const auto start_t_node = node_name_to_number('t', 'a');
    const auto end_t_node = node_name_to_number('t', 'z');

    // Part 1
    std::set<std::tuple<std::uint16_t, std::uint16_t, std::uint16_t>> triples;
    for (auto node = start_t_node; node <= end_t_node; ++node) {
        for (std::uint16_t i { 0 }; i < adjacency_lists[node].size(); ++i) {
            for (std::uint16_t j { static_cast<std::uint16_t>(i + 1) };
                j < adjacency_lists[node].size(); ++j) {
                const auto n1 = adjacency_lists[node][i];
                const auto n2 = adjacency_lists[node][j];
                if (std::ranges::contains(adjacency_lists[n1], n2)) {
                    triples.emplace(sort_tuple(node, n1, n2));
                }
            }
        }
    }
    std::println("Part 1 result: {}", triples.size());

    // Part 2
    // Sort the adjacency lists so that we can use set_intersection
    for (auto& adjacency_list : directed_adjacency_lists) {
        std::ranges::sort(adjacency_list);
    }

    // Filter out unconnected nodes
    std::vector<std::uint16_t> nodes {};
    for (std::uint16_t i { 0 }; i < max_node_nums; ++i) {
        if (!directed_adjacency_lists[i].empty()) {
            nodes.push_back(i);
        }
    }

    auto max_kgraph = find_max_kgraph(nodes, directed_adjacency_lists);
    std::ranges::reverse(max_kgraph);
    std::println("Part 2 result: {}", nodes_to_str(max_kgraph));
    std::println("find_max_kgraph calls: {}", find_max_kgraph_count);

    return 0;
}
