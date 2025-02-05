#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <print>
#include <vector>

static inline std::uint64_t transform(std::uint64_t seed)
{
    static constexpr std::uint64_t prune_modulo = 16777216;
    const auto r1 = ((seed << 6) ^ seed) % prune_modulo;
    const auto r2 = ((r1 >> 5) ^ r1) % prune_modulo;
    const auto r3 = ((r2 << 11) ^ r2) % prune_modulo;
    return r3;
}

static constexpr std::int8_t kOffset { 9 };  // convert the changes from [-9, 9] -> [0, 18]
static constexpr std::uint64_t kBase { 19 }; // enough to hold all digits [0, 18]

static std::uint64_t get_final_secret(std::uint64_t seed, std::uint64_t nrounds)
{
    std::uint64_t rng_state { seed };
    for (std::uint64_t i { 0 }; i < nrounds; ++i) {
        rng_state = transform(rng_state);
    }
    return rng_state;
}

static std::array<std::int8_t, 4> index2changeseq(std::uint64_t index)
{
    std::array<std::int8_t, 4> res;
    res[3] = static_cast<std::int8_t>(index % kBase) - kOffset;
    index /= kBase;
    res[2] = static_cast<std::int8_t>(index % kBase) - kOffset;
    index /= kBase;
    res[1] = static_cast<std::int8_t>(index % kBase) - kOffset;
    index /= kBase;
    res[0] = static_cast<std::int8_t>(index) - kOffset;

    return res;
}

// Update the old change sequence old_index ~ (a, b, c, d) with a price change
static inline std::uint64_t update_changeseq_index(
    std::uint64_t old_index, std::uint64_t prev_price, std::uint64_t current_price)
{
    static constexpr auto bbb = kBase * kBase * kBase;
    const auto change = current_price + kOffset - prev_price;
    return (old_index % bbb) // drop a from (a, b, c, d) -> (b, c, d)
        * kBase              // shift (b, c, d) -> (b, c, d, 0)
        + change;            // add e to (b, c, d, 0) -> (b, c, d, e)
}

static void changeseqs_to_prices(
    std::uint64_t seed, std::uint64_t n, std::vector<std::uint64_t>& acc)
{
    std::uint64_t secret = seed;
    std::uint64_t prev_price = secret % 10;

    std::uint64_t prev_changeseq { 0 };
    for (std::uint64_t i { 0 }; i < 3; ++i) {
        secret = transform(secret);
        const auto price = secret % 10;
        prev_changeseq = update_changeseq_index(prev_changeseq, prev_price, price);

        prev_price = price;
    }

    // Keep track of if we've already sold for a sequence
    std::vector<std::uint8_t> sold(kBase * kBase * kBase * kBase, 0);

    for (std::uint64_t i { 4 }; i <= n; ++i) {
        secret = transform(secret);
        const auto price = secret % 10;
        const auto changeseq = update_changeseq_index(prev_changeseq, prev_price, price);

        if (!sold[changeseq]) {
            acc[changeseq] += price;
            sold[changeseq] = 1;
        }

        prev_price = price;
        prev_changeseq = changeseq;
    }
}

int main()
{
    static constexpr std::uint64_t rounds { 2000 };
    std::uint64_t seed;

    std::uint64_t part1_sum {};
    std::vector<std::uint64_t> seq_to_total_prices(kBase * kBase * kBase * kBase, 0);

    while (std::cin >> seed) {
        // part 1
        part1_sum += get_final_secret(seed, rounds);

        // part 2
        changeseqs_to_prices(seed, rounds, seq_to_total_prices);
    }

    std::println("Part 1 result: {}", part1_sum);

    auto max_price_it = std::max_element(seq_to_total_prices.begin(), seq_to_total_prices.end());
    auto index = std::distance(seq_to_total_prices.begin(), max_price_it);
    const auto best_seq = index2changeseq(static_cast<std::uint64_t>(index));
    std::println("Part 2: best sequence = {},{},{},{}, total prices = {}", best_seq[0], best_seq[1],
        best_seq[2], best_seq[3], *max_price_it);

    return 0;
}
