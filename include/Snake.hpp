#pragma once
#include "Food.hpp"
#include "IAnimationManager.hpp"
#include "ScopedAnimationId.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cstdint>
#include <deque>
#include <memory>

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

private:
    static constexpr float UPDATE_TIME = 100.0f;
    float m_CurrentTime = 0.0f;
};

class GameOverSnakeState : public ISnakeState
{
public:
    virtual ISnakeState* update(Snake& snake, float ms) override;

private:
    ScopedAnimationId m_AnimationId;
    bool m_Initialized{false};
    bool m_AnimationFinished{false};
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
    sf::Vector2f calculateNewHeadPosition(MoveDirection direction) const;
    static MoveDirection getOppositeDirection(MoveDirection direction);
    void setAnimationManager(IAnimationManager* manager);

private:
    std::deque<std::shared_ptr<sf::RectangleShape>> m_BodyPieces;
    sf::RectangleShape* m_Head;
    sf::RectangleShape* m_Tail;
    MoveDirection m_PreviousDirection;
    GameResult m_GameResult;
    Food m_Food;
    ISnakeState* m_State;

    NormalSnakeState m_NormalSnakeState;
    GameOverSnakeState m_GameOverSnakeState;
    IAnimationManager* m_AnimationManager;
    inline static auto const head_color = sf::Color{199U, 177U, 152U};
    inline static auto const body_color = sf::Color{223U, 211U, 195U};
    inline static auto const tail_color = sf::Color{240U, 236U, 227U};
    friend class Food;
    friend class NormalSnakeState;
    friend class GameOverSnakeState;
};
