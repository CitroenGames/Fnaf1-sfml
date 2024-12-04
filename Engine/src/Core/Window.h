#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

constexpr float VIEWPORT_WIDTH = 1024.0f;
constexpr float VIEWPORT_HEIGHT = 576.0f;

class Window {
public:
    static std::shared_ptr<sf::RenderWindow> Init(int width, int height, std::string title);
    static void UpdateViewport();
    static void Destroy();

    static void setCameraPosition(const sf::Vector2f& position) {
        cameraPosition = position;
        // Maintain the current view size while updating the center
        sf::Vector2f currentSize = m_GameView.getSize();
        m_GameView.setSize(currentSize);
        m_GameView.setCenter(cameraPosition);
        m_Window->setView(m_GameView);
    }

    static void setCameraPosition(float x, float y) {
        setCameraPosition(sf::Vector2f(x, y));
    }

    static sf::Vector2f getCameraPosition() {
        return cameraPosition;
    }

    static std::shared_ptr<sf::RenderWindow> const GetWindow() {
        return m_Window;
    }

private:
    static std::shared_ptr<sf::RenderWindow> m_Window;
    static sf::View m_GameView;
    static sf::Vector2f cameraPosition;
};