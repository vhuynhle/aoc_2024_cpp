#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool safe(const std::vector<std::int64_t>& vec)
{
    if (vec.size() <= 1) {
        return true;
    }

    if (vec[0] == vec[1] || std::abs(vec[1] - vec[0]) > 3) {
        return false;
    }

    if (vec[0] < vec[1]) {
        for (std::size_t i { 2 }; i < vec.size(); ++i) {
            if ((vec[i - 1] >= vec[i]) || (vec[i] - vec[i - 1] > 3)) {
                return false;
            }
        }
    } else {
        for (std::size_t i { 2 }; i < vec.size(); ++i) {
            if ((vec[i - 1] <= vec[i]) || (vec[i - 1] - vec[i] > 3)) {
                return false;
            }
        }
    }

    return true;
}

bool almost_safe(const std::vector<std::int64_t>& vec)
{
    for (std::size_t i { 0 }; i < vec.size(); ++i) {
        auto v_copy = vec;
        v_copy.erase(v_copy.begin() + static_cast<std::ptrdiff_t>(i));
        if (safe(v_copy)) {
            return true;
        }
    }

    return false;
}

std::vector<std::int64_t> parse_line(const std::string& input)
{
    std::vector<std::int64_t> result;
    std::int64_t num;
    std::stringstream ss { input };
    while (ss >> num) {
        result.push_back(num);
    }

    return result;
}

int main()
{
    std::string input;
    std::int64_t safe_count { 0 };
    std::int64_t almost_safe_count { 0 };

    while (true) {
        std::getline(std::cin, input);
        const auto numbers = parse_line(input);
        if (numbers.empty()) {
            break;
        }

        if (safe(numbers)) {
            ++safe_count;
            ++almost_safe_count;
        } else if (almost_safe(numbers)) {
            ++almost_safe_count;
        }
    }

    std::cout << "Number of safe lines: " << safe_count << std::endl;
    std::cout << "Number of almost safe lines: " << almost_safe_count << std::endl;
    return 0;
}
