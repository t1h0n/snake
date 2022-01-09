#include "Snake.hpp"
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
        assert(opacity >= 0.0f && opacity <= 1.0F);
        color.a = static_cast<std::uint8_t>(opacity * 255.0F);
        return color;
    }
    void operator()(sf::RectangleShape& obj, const float& opacity_start, const float& opacity_end, float scale_factor)
    {
        obj.setFillColor(change_opacity(obj.getFillColor(), opacity_start * (1 - scale_factor) + opacity_end * scale_factor));
    }
};
using BodyPieceFadeAnimation = ValueSettingAnimationImpl<sf::RectangleShape, float, opacitySetter>;

bool approximatelyEquals(sf::Vector2f const& left, sf::Vector2f const& right)
{
    static auto constexpr PRECISION = 0.005F;
    const auto result = left - right;
    return std::abs(result.x) <= PRECISION && std::abs(result.y) <= PRECISION;
}
} // namespace

ISnakeState* NormalSnakeState::update(Snake& snake, float ms)
{
    m_CurrentTime += ms;
    if (m_CurrentTime >= UPDATE_TIME)
    {
        m_CurrentTime = 0.0F;
        if (snake.getGameResult() == GameResult::None)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && snake.m_PreviousDirection != Snake::getOppositeDirection(MoveDirection::Up))
            {
                snake.move(MoveDirection::Up);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) &&
                     snake.m_PreviousDirection != Snake::getOppositeDirection(MoveDirection::Down))
            {
                snake.move(MoveDirection::Down);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
                     snake.m_PreviousDirection != Snake::getOppositeDirection(MoveDirection::Right))
            {
                snake.move(MoveDirection::Right);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
                     snake.m_PreviousDirection != Snake::getOppositeDirection(MoveDirection::Left))
            {
                snake.move(MoveDirection::Left);
            }
            else
            {
                snake.move(snake.m_PreviousDirection);
            }
        }
        else if (snake.getGameResult() == GameResult::Loss)
        {
            snake.m_GameResult = GameResult::Loss;
            return &snake.m_GameOverSnakeState;
        }
    }
    return &snake.m_NormalSnakeState;
}

ISnakeState* GameOverSnakeState::update(Snake& snake, float /*ms*/)
{
    if (!m_Initialized)
    {
        m_Initialized = true;
        m_AnimationFinished = false;
        std::deque<std::unique_ptr<IAnimation>> fade_out_animation;
        std::deque<std::unique_ptr<IAnimation>> fade_in_animation_short;
        std::deque<std::unique_ptr<IAnimation>> fade_out_animation_short;
        const std::int64_t duration = 300L / static_cast<int>(snake.m_BodyPieces.size());
        for (const auto& body_piece : snake.m_BodyPieces)
        {
            fade_out_animation.push_front(
                std::make_unique<BodyPieceFadeAnimation>(body_piece, 1.0F, 0.2F, std::chrono::milliseconds(duration), EasingType::Linear));
            fade_in_animation_short.push_front(
                std::make_unique<BodyPieceFadeAnimation>(body_piece, 0.2F, 1.0F, std::chrono::milliseconds(duration), EasingType::OutSine));
            fade_out_animation_short.push_front(
                std::make_unique<BodyPieceFadeAnimation>(body_piece, 1.0F, 0.0F, std::chrono::milliseconds(1500), EasingType::InSine));
        }
        auto fade_out_fade_in = std::make_unique<SequenceAnimation>();
        fade_out_fade_in->addAnimationsFromContainer(std::move(fade_out_animation));
        fade_out_fade_in->addAnimations(std::make_unique<WaitAnimation>(std::chrono::seconds(1)));
        fade_out_fade_in->addAnimationsFromContainer(std::move(fade_in_animation_short));

        auto fade_out_short_sequence = std::make_unique<GroupAnimation>();
        fade_out_short_sequence->addAnimationsFromContainer(std::move(fade_out_animation_short));
        fade_out_fade_in->addAnimations(std::make_unique<WaitAnimation>(std::chrono::milliseconds(200)), std::move(fade_out_short_sequence),
                                        std::make_unique<WaitAnimation>(std::chrono::milliseconds(200)));
        fade_out_fade_in->setFinishedCallback([this, &snake]() {
            m_AnimationFinished = true;
            snake.onInit();
        });
        m_AnimationId = snake.m_AnimationManager->addAnimation(std::move(fade_out_fade_in));
    }
    if (m_AnimationFinished)
    {
        m_Initialized = false;
        return &snake.m_NormalSnakeState;
    }

    return &snake.m_GameOverSnakeState;
}

Snake::Snake()
    : m_Head{nullptr}, m_Tail{nullptr}, m_PreviousDirection{MoveDirection::Up}, m_GameResult{GameResult::None},
      m_Food(*this), m_State{nullptr}, m_AnimationManager{nullptr}
{
}

void Snake::setAnimationManager(IAnimationManager* animation_manager)
{
    assert(animation_manager);
    m_AnimationManager = animation_manager;
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
        auto body_part = std::make_shared<sf::RectangleShape>(block_metrics);
        body_part->setPosition({pos.x * block_metrics.x, pos.y * block_metrics.y});
        body_part->setFillColor(body_color);
        m_BodyPieces.push_back(std::move(body_part));
    }
    m_BodyPieces.front()->setFillColor(head_color);
    m_BodyPieces.back()->setFillColor(tail_color);
    m_Head = m_BodyPieces.front().get();
    m_Tail = m_BodyPieces.back().get();
}
void Snake::move(MoveDirection direction)
{
    const sf::Vector2f new_head_pos = calculateNewHeadPosition(direction);

    m_BodyPieces.emplace_front(std::make_shared<sf::RectangleShape>(block_metrics));
    m_Head->setFillColor(body_color);
    m_Head = m_BodyPieces.front().get();
    m_Head->setFillColor(head_color);
    m_Head->setPosition(new_head_pos);
    if (!approximatelyEquals(m_Food.getPosition(), new_head_pos))
    {
        m_BodyPieces.pop_back();
        m_Tail = m_BodyPieces.back().get();
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
        if (m_Head->getPosition() == (*beg)->getPosition())
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
                return {m_Head->getPosition().x, 0.0F};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{0.0F, block_metrics.y};
            }
        case MoveDirection::Up:
            if (m_Head->getPosition().y - block_metrics.y < 0.0F)
            {
                return {m_Head->getPosition().x, static_cast<float>(grid_metrics.y) * block_metrics.y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{0.0F, -block_metrics.y};
            }
        case MoveDirection::Right:
            if (m_Head->getPosition().x + block_metrics.x > static_cast<float>(grid_metrics.x) * block_metrics.x)
            {
                return {0.0F, m_Head->getPosition().y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{block_metrics.x, 0.0F};
            }
        case MoveDirection::Left:
            if (m_Head->getPosition().x - block_metrics.x < 0.0F)
            {
                return {static_cast<float>(grid_metrics.x) * block_metrics.x, m_Head->getPosition().y};
            }
            else
            {
                return m_Head->getPosition() + sf::Vector2f{-block_metrics.x, 0.0F};
            }
        default:
            throw std::runtime_error("Invalid value of move direction");
    }
    return {};
}
MoveDirection Snake::getOppositeDirection(MoveDirection direction)
{
    static constexpr std::array<MoveDirection, 4UL> opposites{MoveDirection::Down, MoveDirection::Up, MoveDirection::Right,
                                                              MoveDirection::Left};
    // using internal layout of enum
    return opposites.at(static_cast<std::size_t>(direction));
}

void Snake::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    struct reverse
    {
        explicit reverse(const std::deque<std::shared_ptr<sf::RectangleShape>>* obj) : obj{obj} {}
        [[nodiscard]] auto begin() const { return obj->crbegin(); }
        [[nodiscard]] auto end() const { return obj->crend(); }

    private:
        std::deque<std::shared_ptr<sf::RectangleShape>> const* obj;
    };
    target.draw(m_Food, states);
    for (auto const& piece : reverse{&m_BodyPieces})
    {
        target.draw(*piece, states);
    }
}
