#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <print>
#include <set>
#include <string>
#include <vector>

static constexpr std::uint64_t nDirections { 4 };
static constexpr auto kULimit { std::numeric_limits<std::uint64_t>::max() };

static inline std::uint8_t direction2num(char direction)
{
    switch (direction) {
    case '>':
        return 0;
    case '^':
        return 1;
    case '<':
        return 2;
    default:
        return 3;
    }
}

static inline char num2direction(std::uint8_t num)
{
    switch (num) {
    case 0:
        return '>';
    case 1:
        return '^';
    case 2:
        return '<';
    default:
        return 'v';
    }
}

/// Represent a location on the grid
struct Coordinate {
    std::uint64_t row;
    std::uint64_t col;
    char direction;
};

/// Represent a neighbor of a node
struct Neighbor {
    std ::uint64_t vertex_index;
    std::uint64_t edge_length;
};

std::vector<std::string> read_grid()
{
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(std::cin, line)) {
        grid.push_back(line);
    }
    return grid;
}

static inline std::uint64_t coord2index(Coordinate coordinate, std::uint64_t ncols)
{
    const auto [r, c, direction] = coordinate;
    const auto d = direction2num(direction);
    return (r * ncols + c) * nDirections + d;
};

static inline Coordinate index2coord(std::uint64_t idx, std::uint64_t ncols)
{
    const std::uint8_t d_num = static_cast<std::uint8_t>(idx % nDirections);
    const char d = num2direction(d_num);
    idx /= nDirections;
    const std::uint64_t c = idx % ncols;
    const std::uint64_t r = idx / ncols;
    return { r, c, d };
};

/// Represent a grid as a graph, using adjacency lists.
/// - Each vertex {row, col, direction} is mapped to an integer index
/// - For each vertex (integer index), keep a list of neighboring vertices together with the edge
/// lengths
/// - Set reverse = true to reverse the edge directions (used in Part 2), where
/// we want to calculate the distance from each node to the target.
std::vector<std::vector<Neighbor>> make_graph(
    const std::vector<std::string>& grid, bool reverse = false)
{
    const auto nrows = grid.size();
    const auto ncols = grid.front().size();
    const auto num_vertices = nrows * ncols * nDirections;
    std::vector<std::vector<Neighbor>> neighbors(num_vertices);

    for (std::uint64_t i { 0 }; i < nrows; ++i) {
        for (std::uint64_t j { 0 }; j < ncols; ++j) {
            if (grid[i][j] == '#') {
                continue;
            }

            // Neighbors within the same cell
            const auto east_index = coord2index({ i, j, '>' }, ncols);
            const auto north_index = east_index + 1;
            const auto west_index = east_index + 2;
            const auto south_index = east_index + 3;
            neighbors[east_index].emplace_back(north_index, 1000);
            neighbors[east_index].emplace_back(south_index, 1000);
            neighbors[north_index].emplace_back(east_index, 1000);
            neighbors[north_index].emplace_back(west_index, 1000);
            neighbors[west_index].emplace_back(north_index, 1000);
            neighbors[west_index].emplace_back(south_index, 1000);
            neighbors[south_index].emplace_back(east_index, 1000);
            neighbors[south_index].emplace_back(west_index, 1000);

            // Neighbors in adjacent cells
            if (grid[i][j + 1] != '#') {
                // Edge from [i, j, >] to [i, j + 1, >]
                const auto source_1 = east_index;
                const auto target_1 = coord2index({ i, j + 1, '>' }, ncols);

                // Edge from [i, j + 1, <] to [i, j, <]
                const auto source_2 = coord2index({ i, j + 1, '<' }, ncols);
                const auto target_2 = west_index;

                if (reverse) {
                    neighbors[target_1].emplace_back(source_1, 1);
                    neighbors[target_2].emplace_back(source_2, 1);
                } else {
                    neighbors[source_1].emplace_back(target_1, 1);
                    neighbors[source_2].emplace_back(target_2, 1);
                }
            }

            if (grid[i + 1][j] != '#') {
                // Edge from [i, j, v] to [i + 1, j, v]
                const auto source_1 = south_index;
                const auto target_1 = coord2index({ i + 1, j, 'v' }, ncols);

                // Edge from [i + 1, j, ^] to [i, j, ^]
                const auto source_2 = coord2index({ i + 1, j, '^' }, ncols);
                const auto target_2 = north_index;

                if (reverse) {
                    neighbors[target_1].emplace_back(source_1, 1);
                    neighbors[target_2].emplace_back(source_2, 1);
                } else {
                    neighbors[source_1].emplace_back(target_1, 1);
                    neighbors[source_2].emplace_back(target_2, 1);
                }
            }
        }
    }

    return neighbors;
}

std::vector<std::uint64_t> dijsktra(const std::vector<std::vector<Neighbor>>& neighbors,
    std::uint64_t num_vertices, std::uint64_t start_node)
{
    std::vector<std::uint64_t> distance(num_vertices, kULimit);
    distance[start_node] = 0;

    const auto compare = [&distance](std::uint64_t node1_index, std::uint64_t node2_index) {
        if (distance[node1_index] != distance[node2_index]) {
            return distance[node1_index] < distance[node2_index];
        }
        // For the set to act as a priority queue, the comparison must be a total order
        return node1_index < node2_index;
    };
    std::set<std::uint64_t, decltype(compare)> queue(compare);
    std::vector<std::uint8_t> done(num_vertices, static_cast<std::uint8_t>(0u));

    for (std::uint64_t idx { 0 }; idx < num_vertices; ++idx) {
        queue.emplace(idx);
    }

    while (!queue.empty()) {
        // Take the current closest node and remove it from the queue
        auto u = *queue.begin();
        queue.erase(queue.begin());
        done[u] = 1;

        for (auto [v, v_distance] : neighbors[u]) {
            if (done[v]) {
                continue;
            }

            const auto alt = distance[u] + v_distance;
            if (alt < distance[v]) {
                // Update the queue. Note: Remove it before update distance[] to ensure
                // consistent priority
                auto it = queue.find(v);
                queue.erase(it);
                distance[v] = alt;
                queue.emplace(v);
            }
        }
    }

    return distance;
}

int main()
{

    const std::vector<std::string> grid { read_grid() };
    const std::uint64_t nrows { grid.size() };
    const std::uint64_t ncols { grid.front().size() };

    // Part 1
    // Represent each pair {cell on the grid, direction} as a vertex of a graph.
    // The shortest distance can then be found using Dijsktra's algorithm
    const auto neighbors = make_graph(grid);

    const std::uint64_t num_vertices = nrows * ncols * nDirections;
    const std::uint64_t start_row = nrows - 2;
    const std::uint64_t start_col = 1;
    const char start_direction = '>';
    const std::uint64_t start_vertex
        = coord2index({ start_row, start_col, start_direction }, ncols);

    const std::uint64_t target_row = 1;
    const std::uint64_t target_col = ncols - 2;
    const auto target_east = coord2index({ target_row, target_col, '>' }, ncols);
    const auto target_north = target_east + 1;
    const auto target_west = target_east + 2;
    const auto target_south = target_east + 3;
    const std::vector<std::uint64_t> targets { target_east, target_north, target_west,
                                               target_south };

    const auto distances = dijsktra(neighbors, num_vertices, start_vertex);
    std::uint64_t best_target = target_east;
    std::uint64_t best_distance = kULimit;
    for (auto target : { target_east, target_north, target_west, target_south }) {
        const auto d = distances[target];
        if (d < best_distance) {
            best_target = target;
            best_distance = d;
        }
    }

    const auto [rtarget, ctarget, dtarget] = index2coord(best_target, ncols);
    std::println(
                 "Part 1 result: target {} {} {}, distance {}", rtarget, ctarget, dtarget, best_distance);

    // Part 2
    // Calculate the distance from each cell to the target
    // A cell x is on a best path if:
    // distance[start, x] + distance[x, target] = distance[start, target]
    const auto rneighbors = make_graph(grid, true);
    const auto rdistances = dijsktra(rneighbors, num_vertices, best_target);

    std::set<std::uint64_t> cells_on_best_paths {};
    for (std::uint64_t vertex_idx { 0 }; vertex_idx < num_vertices; ++vertex_idx) {
        if (distances[vertex_idx] != kULimit
            && rdistances[vertex_idx] != kULimit
            && distances[vertex_idx] + rdistances[vertex_idx] == best_distance) {
            // The vertex is on the path
            const auto [r, c, direction] = index2coord(vertex_idx, ncols);
            cells_on_best_paths.emplace(r * ncols + c);
        }
    }

    std::println("Number of cells on a best path: {}", cells_on_best_paths.size());

    return 0;
}
