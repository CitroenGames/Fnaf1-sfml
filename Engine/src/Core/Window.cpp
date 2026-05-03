#include "Window.h"

#include <SFML/Window/VideoMode.hpp>

std::shared_ptr<sf::RenderWindow> Window::m_Window = nullptr;
sf::View Window::m_GameView;

std::shared_ptr<sf::RenderWindow> Window::Init(int width, int height, const std::string &title) {
    if (m_Window) {
        return m_Window;
    }

    m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title);
    m_GameView.setSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    m_GameView.setCenter(sf::Vector2f(VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f));
    m_Window->setView(m_GameView);
    return m_Window;
}

void Window::UpdateViewport() {
    if (!m_Window) {
        return;
    }

    const sf::Vector2u windowSize = m_Window->getSize();
    if (windowSize.x == 0 || windowSize.y == 0) {
        return;
    }

    const float windowAspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const float viewportAspectRatio = VIEWPORT_WIDTH / VIEWPORT_HEIGHT;

    float viewWidth = VIEWPORT_WIDTH;
    float viewHeight = VIEWPORT_HEIGHT;

    if (windowAspectRatio > viewportAspectRatio) {
        // Window is wider than viewport
        viewWidth = viewHeight * windowAspectRatio;
    } else {
        // Window is taller than viewport
        viewHeight = viewWidth / windowAspectRatio;
    }

    // Keep the current camera position when updating the viewport
    const sf::Vector2f currentCenter = m_GameView.getCenter();
    m_GameView.setSize(viewWidth, viewHeight);
    m_GameView.setCenter(currentCenter);
    m_Window->setView(m_GameView);
}

void Window::Destroy() {
    if (!m_Window) {
        return;
    }

    if (m_Window->isOpen()) {
        m_Window->close();
    }

    m_Window.reset();
}
