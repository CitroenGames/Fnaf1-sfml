#pragma once

constexpr float VIEWPORT_WIDTH = 1280.0f;
constexpr float VIEWPORT_HEIGHT = 720.0f;

class Window {
public:
    static std::shared_ptr<sf::RenderWindow> Init(int width, int height, std::string title);
    static void UpdateViewport();
    static void Destroy();

    static std::shared_ptr<sf::RenderWindow> const GetWindow() {
        return m_Window;
    }

private:
    static std::shared_ptr<sf::RenderWindow> m_Window;
    static sf::View m_GameView;
};