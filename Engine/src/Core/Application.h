#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>

class Application {
public:
    static void Init(int width = 1280, int height = 720, const std::string &title = "Window");

    static void Run();

    static void Destroy();

private:
    static std::shared_ptr<sf::RenderWindow> m_Window;
};
