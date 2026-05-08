#pragma once

#include <string>

#include <SFML/Graphics/Color.hpp>

#include "Core/Window.h"

class Application {
public:
    struct Config {
        Window::Config window;
        int fixedTickRate = 66;
        bool verticalSync = true;
        bool enableImGui = true;
        sf::Color clearColor = sf::Color::Black;
    };

    static void Init(int width = 1280, int height = 720, const std::string &title = "Window");
    static void Init(const Config &config);
    static Config NativeResolutionConfig(
        int width,
        int height,
        const std::string &title,
        int fixedTickRate = 60);

    static void Run();

    static void Destroy();

    static const Config &GetConfig();
};
