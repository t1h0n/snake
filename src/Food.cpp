#include "Food.hpp"
#include "Common.hpp"
#include "Snake.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

Food::Food(Snake const& snake) : m_FoodGenerator(snake), m_FoodSprite(block_metrics)
{
    m_FoodSprite.setFillColor(food_color);
}

static bool approximatelyEquals(sf::Vector2f const& left, sf::Vector2f const& right)
{
    static auto constexpr PRECISION = 0.005f;
    const auto result = left - right;
    return std::abs(result.x) <= PRECISION && std::abs(result.y) <= PRECISION;
}

std::optional<sf::Vector2f> Food::FoodGenerator::generatePosition()
{
    std::vector<sf::Vector2f> possible_positions;
    possible_positions.reserve(grid_metrics.x * grid_metrics.y);
    for (std::int32_t i = 0; i < grid_metrics.x; ++i)
    {
        for (std::int32_t j = 0; j < grid_metrics.y; ++j)
        {
            auto const possible_position =
                sf::Vector2f{static_cast<float>(i) * block_metrics.x, static_cast<float>(j) * block_metrics.y};
            bool found = false;
            for (auto const& body_piece : snake.m_BodyPieces)
            {
                if (approximatelyEquals(body_piece->getPosition(), possible_position))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                possible_positions.push_back(possible_position);
            }
        }
    }
    if (possible_positions.size() == 0U)
    {
        return std::nullopt;
    }
    return possible_positions[Food::FoodGenerator::generator(0U, possible_positions.size() - 1U)()];
}

void Food::draw(sf::RenderTarget& target, sf::RenderStates states) const { target.draw(m_FoodSprite, states); }

bool Food::generateNewPosition()
{
    const auto new_food_position = m_FoodGenerator.generatePosition();
    if (new_food_position)
    {
        m_FoodSprite.setPosition(*new_food_position);
        return true;
    }
    else
    {
        return false;
    }
}
const sf::Vector2f& Food::getPosition() const { return m_FoodSprite.getPosition(); }