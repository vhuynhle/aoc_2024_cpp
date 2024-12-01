#include "similarity.hpp"

#include <iostream>
#include <print>
#include <vector>

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

    std::println("Similarity score: {}", similarity_score(v1, v2));

    return 0;
}
