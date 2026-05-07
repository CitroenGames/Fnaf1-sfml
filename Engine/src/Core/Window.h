#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>

constexpr float VIEWPORT_WIDTH = 1280.0f;
constexpr float VIEWPORT_HEIGHT = 720.0f;

class Window {
public:
    static std::shared_ptr<sf::RenderWindow> Init(int width, int height, const std::string &title);

    static void UpdateViewport();

    static void Destroy();

    static std::shared_ptr<sf::RenderWindow> GetWindow();
};
