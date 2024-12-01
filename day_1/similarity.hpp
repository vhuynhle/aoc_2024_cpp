#pragma once

#include <cstdint>
#include <span>

std::int64_t similarity_score(std::span<const std::int64_t> v1, std::span<const std::int64_t> v2);
