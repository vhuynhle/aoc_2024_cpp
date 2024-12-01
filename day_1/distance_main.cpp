#include "distance.hpp"

#include <iostream>
#include <print>

int main()
{
    std::vector<std::int64_t> v1;
    std::vector<std::int64_t> v2;

    std::int64_t pos1;
    std::int64_t pos2;
    while ((std::cin >> pos1 >> pos2)) {
        v1.push_back(pos1);
        v2.push_back(pos2);
    }

    std::println("Total distance: {}", distance(std::move(v1), std::move(v2)));

    return 0;
}
