#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

struct BoardMetrics
{
    std::uint32_t width, height;
};
auto const block_metrics = sf::Vector2f{64.0f, 64.0f};
auto constexpr board_metrics = BoardMetrics{640u, 640u};
// clang-format off
auto const grid_metrics = sf::Vector2<std::int32_t>{ static_cast<std::int32_t>(board_metrics.height) / static_cast<std::int32_t>(block_metrics.x) - 1,
                                                     static_cast<std::int32_t>(board_metrics.width) / static_cast<std::int32_t>(block_metrics.y) - 1}; // starting form zero
// clang-format on