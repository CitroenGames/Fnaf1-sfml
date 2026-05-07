#include "Window.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace {
    std::shared_ptr<sf::RenderWindow> g_Window;
    sf::View g_GameView;
}

std::shared_ptr<sf::RenderWindow> Window::Init(int width, int height, const std::string &title) {
    if (g_Window) {
        return g_Window;
    }

    g_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title);
    g_GameView.setSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_GameView.setCenter(sf::Vector2f(VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f));
    g_Window->setView(g_GameView);
    return g_Window;
}

void Window::UpdateViewport() {
    if (!g_Window) {
        return;
    }

    const sf::Vector2u windowSize = g_Window->getSize();
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

    const sf::Vector2f currentCenter = g_GameView.getCenter();
    g_GameView.setSize(viewWidth, viewHeight);
    g_GameView.setCenter(currentCenter);
    g_Window->setView(g_GameView);
}

void Window::Destroy() {
    if (!g_Window) {
        return;
    }

    if (g_Window->isOpen()) {
        g_Window->close();
    }

    g_Window.reset();
}

std::shared_ptr<sf::RenderWindow> Window::GetWindow() {
    return g_Window;
}
