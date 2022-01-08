#include "Snake.hpp"
#include "CAnimationManager.hpp"
#include "Common.hpp"
#include "DoActionAnimation.hpp"
#include "GroupAnimation.hpp"
#include "SequenceAnimation.hpp"
#include "ValueSettingAnimation.hpp"
#include "WaitAnimation.hpp"
#include <cassert>
#include <iostream>
namespace
{
struct opacitySetter
{
    static sf::Color change_opacity(sf::Color color, float opacity)
    {
        const auto new_opacity = static_cast<std::uint32_t>(opacity * 255.0f);
        color.a = [new_opacity]() {
            if (new_opacity >= 0)
            {
                if (new_opacity <= 255)
                    return static_cast<std::uint8_t>(new_opacity);
                else
                    return static_cast<std::uint8_t>(255);
            }
            else
            {
                return static_cast<std::uint8_t>(0);
            }
        }();
        return color;
    }
    void operator()(sf::RectangleShape& obj, const float& opacity)
    {
        obj.setFillColor(change_opacity(obj.getFillColor(), opacity));
    }
};
using BodyPieceFadeAnimation = ValueSettingAnimationImpl<sf::RectangleShape, float, opacitySetter>;

bool approximatelyEquals(sf::Vector2f const& left, sf::Vector2f const& right)
{
    static auto constexpr PRECISION = 0.005f;
    const auto result = left - right;
    return std::abs(result.x) <= PRECISION && std::abs(result.y) <= PRECISION;
}
} // namespace

ISnakeState* NormalSnakeState::update(Snake& snake, float ms)
{
    m_CurrentTime += ms;
    if (m_CurrentTime >= UPDATE_TIME)
    {
        m_CurrentTime = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) &&
            snake.m_PreviousDirection != snake.getOppositeDirection(MoveDirection::Up))
        {
            snake.move(MoveDirection::Up);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
                 snake.m_PreviousDirection != snake.getOppositeDirection(MoveDirection::Down))
        {
            snake.move(MoveDirection::Down);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
                 snake.m_PreviousDirection != snake.getOppositeDirection(MoveDirection::Right))
        {
            snake.move(MoveDirection::Right);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
                 snake.m_PreviousDirection != snake.getOppositeDirection(MoveDirection::Left))
        {
            snake.move(MoveDirection::Left);
        }
        else
        {
            snake.move(snake.m_PreviousDirection);
        }

        if (snake.getGameResult() == GameResult::Loss)
        {
            return &snake.m_GameOverSnakeState;
        }
    }
    return &snake.m_NormalSnakeState;
}

ISnakeState* GameOverSnakeState::update(Snake& snake, float ms)
{
    auto head = std::shared_ptr<sf::RectangleShape>(&snake.m_BodyPieces.front(), [](auto*) {});
    auto manager = CAnimationManager();
    const auto id = manager.addAnimation(std::make_unique<GroupAnimation>(std::make_unique<SequenceAnimation>(
        std::make_unique<BodyPieceFadeAnimation>(head, 1.0f, 0.0f, std::chrono::milliseconds(2)),
        std::make_unique<DoActionAnimation>([]() { std::cout << "works\n"; }),
        std::make_unique<WaitAnimation>(std::chrono::milliseconds(10)))));

    manager.play(std::chrono::milliseconds(10));
    m_Time += ms;
    if (m_Time > UPDATE_TIME / 2.0f)
    {
        m_Time = 0.0f;
        for (auto& snake_piece : snake.m_BodyPieces)
        {
            const auto change_opacity = [](sf::Color color, float opacity) {
                const auto new_opacity = static_cast<std::uint32_t>(opacity * 255.0f);
                color.a = [new_opacity]() {
                    if (new_opacity >= 0)
                    {
                        if (new_opacity <= 255)
                            return static_cast<std::uint8_t>(new_opacity);
                        else
                            return static_cast<std::uint8_t>(255);
                    }
                    else
                    {
                        return static_cast<std::uint8_t>(0);
                    }
                }();
                return color;
            };
            snake_piece.setFillColor(change_opacity(snake_piece.getFillColor(), 1.0f));
        }
        snake.onInit();
        return &snake.m_NormalSnakeState;
    }
    if (m_FadeOut)
    {
        m_CurrentOpacity -= ms / 1e3f;
        if (m_CurrentOpacity < 0.3f)
        {
            m_CurrentOpacity = 0.3f;
            m_FadeOut = false;
        }
    }
    else
    {
        m_CurrentOpacity += ms / 1e3f;
        if (m_CurrentOpacity >= 1.0f)
        {
            m_CurrentOpacity = 1.0f;
            m_FadeOut = true;
        }
    }
    for (auto& snake_piece : snake.m_BodyPieces)
    {
        const auto change_opacity = [](sf::Color color, float opacity) {
            const auto new_opacity = static_cast<std::uint32_t>(opacity * 255.0f);
            color.a = [new_opacity]() {
                if (new_opacity >= 0)
                {
                    if (new_opacity <= 255)
                        return static_cast<std::uint8_t>(new_opacity);
                    else
                        return static_cast<std::uint8_t>(255);
                }
                else
                {
                    return static_cast<std::uint8_t>(0);
                }
            }();
            return color;
        };
        snake_piece.setFillColor(change_opacity(snake_piece.getFillColor(), m_CurrentOpacity));
    }
    return &snake.m_GameOverSnakeState;
}

Snake::Snake()
    : m_Head{nullptr}, m_Tail{nullptr}, m_PreviousDirection{MoveDirection::Up}, m_GameResult{GameResult::None},
      m_Food(*this), m_State{nullptr}
{
}

void Snake::onInit()
{
    m_BodyPieces.clear();
    setBodyPieces({{1, 1}, {1, 2}, {1, 3}});
    m_PreviousDirection = MoveDirection::Up;
    m_GameResult = GameResult::None;
    m_State = &m_NormalSnakeState;
    m_Food.generateNewPosition();
}

void Snake::onUpdate(float ms) { m_State = m_State->update(*this, ms); }
void Snake::setBodyPieces(std::deque<sf::Vector2<std::int32_t>> const& initial_position)
{
    assert(initial_position.size() > 2);
    for (auto const& pos : initial_position)
    {
        auto body_part = sf::RectangleShape(block_metrics);
        body_part.setPosition({pos.x * block_metrics.x, pos.y * block_metrics.y});
        body_part.setFillColor(body_color);
        m_BodyPieces.push_back(std::move(body_part));
    }
    m_BodyPieces.front().setFillColor(head_color);
    m_BodyPieces.back().setFillColor(tail_color);
    m_Head = &m_BodyPieces.front();
    m_Tail = &m_BodyPieces.back();
}
void Snake::move(MoveDirection direction)
{
    const sf::Vector2f new_head_pos = calculateNewHeadPosition(direction);

    m_BodyPieces.emplace_front(block_metrics);
    m_Head->setFillColor(body_color);
    m_Head = &m_BodyPieces.front();
    m_Head->setFillColor(head_color);
    m_Head->setPosition(new_head_pos);
    if (!approximatelyEquals(m_Food.getPosition(), new_head_pos))
    {
        m_BodyPieces.pop_back();
        m_Tail = &m_BodyPieces.back();
        m_Tail->setFillColor(tail_color);
    }
    else
    {
        if (!m_Food.generateNewPosition())
        {
            m_GameResult = GameResult::Win;
        }
    }
    m_PreviousDirection = direction;
}
GameResult Snake::getGameResult() const
{
    if (m_GameResult != GameResult::None)
    {
        return m_GameResult;
    }
    assert(m_BodyPieces.size() > 1);
    auto beg = ++m_BodyPieces.cbegin();
    auto const end = m_BodyPieces.cend();

    for (; beg != end; ++beg)
    {
        if (m_Head->getPosition() == beg->getPosition())
        {
            return GameResult::Loss;
        }
    }
    return GameResult::None;
}

sf::Vector2f Snake::calculateNewHeadPosition(MoveDirection direction) const
{
    switch (direction)
    {
        case MoveDirection::Down:
            if (m_Head->getPosition().y + block_metrics.y > static_cast<float>(grid_metrics.y) * block_metrics.y)
            {
                return {m_Head->getPosition().x, 0.0f};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{0.0f, block_metrics.y};
            }
        case MoveDirection::Up:
            if (m_Head->getPosition().y - block_metrics.y < 0.0f)
            {
                return {m_Head->getPosition().x, static_cast<float>(grid_metrics.y) * block_metrics.y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{0.0f, -block_metrics.y};
            }
        case MoveDirection::Right:
            if (m_Head->getPosition().x + block_metrics.x > static_cast<float>(grid_metrics.x) * block_metrics.x)
            {
                return {0.0f, m_Head->getPosition().y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{block_metrics.x, 0.0f};
            }
        case MoveDirection::Left:
            if (m_Head->getPosition().x - block_metrics.x < 0.0f)
            {
                return {static_cast<float>(grid_metrics.x) * block_metrics.x, m_Head->getPosition().y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{-block_metrics.x, 0.0f};
            }
        default:
            throw std::runtime_error("Invalid value of move direction");
    }
    return {};
}
MoveDirection Snake::getOppositeDirection(MoveDirection direction) const
{
    static constexpr std::array<MoveDirection, 4UL> opposites{MoveDirection::Down, MoveDirection::Up,
                                                              MoveDirection::Right, MoveDirection::Left};
    // using internal layout of enum
    return opposites.at(static_cast<std::size_t>(direction));
}

void Snake::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    struct reverse
    {
        reverse(const std::deque<sf::RectangleShape>* obj) : obj{obj} {}
        auto begin() { return obj->crbegin(); }
        auto end() { return obj->crend(); }
        std::deque<sf::RectangleShape> const* obj;
    };
    target.draw(m_Food, states);
    for (auto const& piece : reverse{&m_BodyPieces})
    {
        target.draw(piece, states);
    }
}
