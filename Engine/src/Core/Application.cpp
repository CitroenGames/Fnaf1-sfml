#include "Application.h"

#include <algorithm>
#include <chrono>
#include <memory>

#include "Assets/Resources.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Input/InputActionMap.h"
#include "Scene/SceneManager.h"
#include "imgui/imgui-SFML.h"
#include "Utils/Profiler.h"

namespace {
    std::shared_ptr<sf::RenderWindow> g_Window;
    Application::Config g_Config;
}

void Application::Init(int width, int height, const std::string &title) {
    const int safeWidth = std::max(width, 1);
    const int safeHeight = std::max(height, 1);
    Config config;
    config.window.width = safeWidth;
    config.window.height = safeHeight;
    config.window.title = title;
    config.window.designResolution = {
        static_cast<float>(safeWidth),
        static_cast<float>(safeHeight)
    };
    Init(config);
}

void Application::Init(const Config &config) {
    g_Config = config;
    if (g_Config.fixedTickRate <= 0) {
        g_Config.fixedTickRate = 66;
    }

    g_Window = Window::Init(g_Config.window);
    g_Window->setVerticalSyncEnabled(g_Config.verticalSync);
    if (g_Config.enableImGui) {
        ImGui::SFML::Init(*g_Window, true);
    }
    Window::UpdateViewport();
}

Application::Config Application::NativeResolutionConfig(
    int width,
    int height,
    const std::string &title,
    int fixedTickRate) {
    const int safeWidth = std::max(width, 1);
    const int safeHeight = std::max(height, 1);

    Config config;
    config.window.width = safeWidth;
    config.window.height = safeHeight;
    config.window.title = title;
    config.window.designResolution = {
        static_cast<float>(safeWidth),
        static_cast<float>(safeHeight)
    };
    config.window.scaleMode = Window::ScaleMode::Letterbox;
    config.fixedTickRate = fixedTickRate > 0 ? fixedTickRate : 60;
    return config;
}

void Application::Run() {
    if (!g_Window) {
        return;
    }

    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    const double frameTime = 1.0 / static_cast<double>(g_Config.fixedTickRate);

    sf::Event event;
    sf::Clock deltaClock;

    while (g_Window->isOpen()) {
        PROFILE_BEGIN("Application Loop");
        while (g_Window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                g_Window->close();
                break;
            } else if (event.type == sf::Event::Resized) {
                Window::UpdateViewport();
            }
            if (g_Config.enableImGui) {
                ImGui::SFML::ProcessEvent(event);
            }
        }

        Input::Update();

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;
        accumulator += elapsedTime.count();

        if (g_Config.enableImGui) {
            ImGui::SFML::Update(*g_Window, deltaClock.restart());
        }

        while (accumulator >= frameTime) {
            SceneManager::FixedUpdate();
            accumulator -= frameTime;
        }

        double deltaTime = elapsedTime.count();
        SceneManager::Update(deltaTime);

        g_Window->clear(g_Config.clearColor);
        LayerManager::Draw(*g_Window);
        SceneManager::Render();
        if (g_Config.enableImGui) {
            ImGui::SFML::Render(*g_Window);
        }
        g_Window->display();
        PROFILE_END();
    }

    Application::Destroy();
}

void Application::Destroy() {
    PROFILE_BEGIN("Application Shutdown");
    LayerManager::Clear();
    SceneManager::Destroy();
    if (g_Config.enableImGui) {
        ImGui::SFML::Shutdown();
    }
    Resources::Unload();
    Window::Destroy();
    g_Window.reset();
    PROFILE_END();
}

const Application::Config &Application::GetConfig() {
    return g_Config;
}
