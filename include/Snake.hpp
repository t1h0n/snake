#pragma once
#include "Food.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cstdint>
#include <deque>

class Snake;

enum class MoveDirection : std::int32_t
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
};
enum class GameResult : std::int32_t
{
    None = 0,
    Win = 1,
    Loss = 2
};
class ISnakeState
{
public:
    virtual ISnakeState* update(Snake& snake, float ms) = 0;
    virtual ~ISnakeState() = default;
};

class NormalSnakeState : public ISnakeState
{
public:
    virtual ISnakeState* update(Snake& snake, float ms) override;
    static constexpr float UPDATE_TIME = 100.0f;
    float m_CurrentTime = 0.0f;
};

class GameOverSnakeState : public ISnakeState
{
public:
    virtual ISnakeState* update(Snake& snake, float ms) override;
    static constexpr float UPDATE_TIME = 1e4f;
    float m_CurrentOpacity = 1.0f;
    float m_Time = 0.0f;
    bool m_FadeOut = true;
};

class Snake : public sf::Drawable
{
public:
    Snake();
    void onInit();
    void onUpdate(float ms);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setBodyPieces(std::deque<sf::Vector2<std::int32_t>> const& initial_position);
    void move(MoveDirection direction);
    GameResult getGameResult() const;
    sf::Vector2f calculateNewHeadPosition(MoveDirection direction)const ;
    MoveDirection getOppositeDirection(MoveDirection direction)const ;

private:
    std::deque<sf::RectangleShape> m_BodyPieces;
    sf::RectangleShape* m_Head;
    sf::RectangleShape* m_Tail;
    MoveDirection m_PreviousDirection;
    GameResult m_GameResult;
    Food m_Food;
    ISnakeState* m_State;

    NormalSnakeState m_NormalSnakeState;
    GameOverSnakeState m_GameOverSnakeState;
    inline static auto const head_color = sf::Color{199U, 177U, 152U};
    inline static auto const body_color = sf::Color{223U, 211U, 195U};
    inline static auto const tail_color = sf::Color{240U, 236U, 227U};
    friend class Food;
    friend class NormalSnakeState;
    friend class GameOverSnakeState;
};
