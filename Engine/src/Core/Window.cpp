#include "Window.h"

std::shared_ptr<sf::RenderWindow> Window::m_Window = nullptr;
sf::View Window::m_GameView;

std::shared_ptr<sf::RenderWindow> Window::Init(int width, int height, std::string title) {
    if (m_Window)
        return m_Window;

    m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)), title);
    m_GameView.setSize(sf::Vector2f(VIEWPORT_WIDTH, VIEWPORT_HEIGHT));

    m_GameView.setCenter(sf::Vector2f(VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f));
    m_Window->setView(m_GameView);
    return m_Window;
}

void Window::UpdateViewport() {
    sf::Vector2u windowSize = m_Window->getSize();
    float windowAspectRatio = windowSize.x / (float) windowSize.y;
    float viewportAspectRatio = VIEWPORT_WIDTH / VIEWPORT_HEIGHT;

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
    sf::Vector2f currentCenter = m_GameView.getCenter();
    m_GameView.setSize(sf::Vector2f(viewWidth, viewHeight));
    m_GameView.setCenter(currentCenter);
    m_Window->setView(m_GameView);
}

void Window::Destroy() {
    m_Window->close();
    m_Window.reset();
}
