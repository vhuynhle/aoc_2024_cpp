#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

std::size_t count_substr(std::string_view str, std::string_view substr)
{
    std::size_t count { 0 };
    std::size_t pos { 0 };
    while (true) {
        pos = str.find(substr, pos);
        if (pos == std::string_view::npos) {
            break;
        }

        ++count;
        pos += substr.length();
    }

    return count;
}

std::string get_col(const std::vector<std::string>& arr, std::size_t col)
{
    std::string res {};
    res.reserve(arr.size());
    for (const auto& row : arr) {
        res.push_back(row[col]);
    }
    return res;
}

std::string get_forward_diagonal(const std::vector<std::string>& arr, std::size_t d)
{
    const std::size_t m = arr.size();
    const std::size_t n = arr[0].size();
    const std::size_t start_r = (d < m) ? d : (m - 1);
    const std::size_t end_r = std::max(std::size_t { 0 }, d - (n - 1));

    std::string res {};
    for (std::size_t r { start_r }; (r >= end_r) && (r <= start_r); --r) {
        const std::size_t c = d - r;
        res.push_back(arr[r][c]);
    }
    return res;
}

std::string get_backward_diagonal(const std::vector<std::string>& arr, size_t d)
{
    // Diagonal d:
    // (r - c) + (n - 1) = d
    const std::size_t m = arr.size();
    const std::size_t n = arr[0].size();
    const std::size_t start_c = (d < n) ? (n - 1 - d) : 0;
    const std::size_t end_c = std::min(n - 1, (m - 1) + (n - 1) - d);

    std::string res {};
    for (std::size_t c { start_c }; c <= end_c; ++c) {
        const std::size_t r = c - ((n - 1) - d);
        res.push_back(arr[r][c]);
    }

    return res;
}

std::size_t count_str(const std::vector<std::string>& arr, const std::string& str)
{
    if (arr.empty() || arr[0].empty()) {
        return 0;
    }

    const std::string rstr { str.rbegin(), str.rend() };
    const auto nrows = arr.size();
    const auto ncols = arr[0].length();

    std::size_t result { 0 };

    for (const auto& row : arr) {
        result += count_substr(row, str);
        result += count_substr(row, rstr);
    }

    for (std::size_t i { 0 }; i < ncols; ++i) {
        const std::string col = get_col(arr, i);
        result += count_substr(col, str);
        result += count_substr(col, rstr);
    }

    // Diagonal x + y = d;
    for (std::size_t d { 0 }; d < nrows + ncols - 1; ++d) {
        const auto diagonal = get_forward_diagonal(arr, d);
        result += count_substr(diagonal, str);
        result += count_substr(diagonal, rstr);
    }

    // Diagonal r - c + (n - 1) = d
    for (std::size_t d { 0 }; d < nrows + ncols - 1; ++d) {
        const auto diagonal = get_backward_diagonal(arr, d);
        result += count_substr(diagonal, str);
        result += count_substr(diagonal, rstr);
    }

    return result;
}

std::size_t count_xmas(const std::vector<std::string>& arr)
{
    const auto is_xmas = [](const std::string& s) -> bool { return s == "MAS" || s == "SAM"; };

    std::size_t count { 0 };
    for (std::size_t r { 0 }; r + 2 < arr.size(); ++r) {
        for (std::size_t c { 0 }; c + 2 < arr[0].size(); ++c) {
            const std::string diag1 { arr[r][c], arr[r + 1][c + 1], arr[r + 2][c + 2] };
            const std::string diag2 { arr[r + 2][c], arr[r + 1][c + 1], arr[r][c + 2] };
            if (is_xmas(diag1) && is_xmas(diag2)) {
                ++count;
            }
        }
    }
    return count;
}

int main()
{
    std::string line;
    std::vector<std::string> arr;
    while (std::getline(std::cin, line)) {
        arr.push_back(line);
    }

    std::cout << "Result part 1: " << count_str(arr, "XMAS") << std::endl;
    std::cout << "Result part 2: " << count_xmas(arr) << std::endl;
    return 0;
}
