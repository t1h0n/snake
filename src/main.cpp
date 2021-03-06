#include "AnimationManager.hpp"
#include "Common.hpp"
#include "Food.hpp"
#include "Snake.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

class Game
{
public:
    Game() = default;
    void run()
    {
        init();
        auto start_time = std::chrono::steady_clock::now();
        while (m_Window->isOpen())
        {
            const auto current_time = std::chrono::steady_clock::now();
            // clang-format off
            const float elapsed_time = std::chrono::duration_cast<std::chrono::duration<float, std::chrono::milliseconds::period>>(current_time - start_time).count();
            // clang-format on
            sf::Event event;
            while (m_Window->pollEvent(event))
            {
                processWindowInput(event);
            }
            update(elapsed_time);
            m_AnimatinManager.play(std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time));
            render();
            start_time = current_time;
        }
    }

private:
    void processWindowInput(sf::Event event)
    {
        if (event.type == sf::Event::Closed)
        {
            m_Window->close();
        }
    }
    void init()
    {
        m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(board_metrics.width, board_metrics.height), sf::String{"Snake"},
                                                      sf::Style::Titlebar | sf::Style::Close);
        m_Window->setFramerateLimit(60);
        m_Snake.onInit();
        m_Snake.setAnimationManager(&m_AnimatinManager);
    }
    void update(float ms) { m_Snake.onUpdate(ms); }
    void render()
    {
        m_Window->clear();
        m_Window->draw(m_Snake);
        m_Window->display();
    }

private:
    std::unique_ptr<sf::RenderWindow> m_Window;
    Snake m_Snake;
    animation::AnimationManager m_AnimatinManager;
};

int main()
{
    Game game;
    game.run();
    return 0;
}