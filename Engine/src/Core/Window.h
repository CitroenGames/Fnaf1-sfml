#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

constexpr float VIEWPORT_WIDTH = 1280.0f;
constexpr float VIEWPORT_HEIGHT = 720.0f;

class Window {
public:
    enum class ScaleMode {
        Expand,
        Letterbox,
        Stretch
    };

    struct Config {
        int width = 1280;
        int height = 720;
        std::string title = "Window";
        sf::Vector2f designResolution = {VIEWPORT_WIDTH, VIEWPORT_HEIGHT};
        ScaleMode scaleMode = ScaleMode::Expand;
    };

    static std::shared_ptr<sf::RenderWindow> Init(int width, int height, const std::string &title);
    static std::shared_ptr<sf::RenderWindow> Init(const Config &config);

    static void UpdateViewport();

    static void Destroy();

    static std::shared_ptr<sf::RenderWindow> GetWindow();

    static const Config &GetConfig();
    static sf::Vector2f GetDesignResolution();
    static ScaleMode GetScaleMode();
    static void SetDesignResolution(sf::Vector2f resolution);
    static void SetScaleMode(ScaleMode scaleMode);
    static sf::View GetDesignView();
};
