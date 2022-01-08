#pragma once
#include "Common.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <optional>
#include <random>

class Snake;
class Food : public sf::Drawable
{
public:
    Food(Snake const& snake);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    const sf::Vector2f& getPosition() const;
    bool generateNewPosition();

private:
    class FoodGenerator
    {
    public:
        FoodGenerator(Snake const& snake) : snake{snake} {}
        std::optional<sf::Vector2f> generatePosition();

    private:
        struct generator
        {
            generator(std::size_t beg, std::size_t end) : random_engine{rand_dev()}, uniform_dist{beg, end} {}
            std::random_device rand_dev;
            std::default_random_engine random_engine;
            std::uniform_int_distribution<std::size_t> uniform_dist;
            std::size_t operator()() { return uniform_dist(random_engine); }
        };

    private:
        Snake const& snake;
    };
    FoodGenerator m_FoodGenerator;
    sf::RectangleShape m_FoodSprite;
    inline static auto const food_color = sf::Color{166U, 141U, 173U};
};