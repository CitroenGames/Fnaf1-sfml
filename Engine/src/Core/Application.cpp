#include "Application.h"

#include <chrono>
#include <memory>

#include "Assets/Resources.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Scene/SceneManager.h"
#include "imgui/imgui-SFML.h"
#include "Utils/Profiler.h"

namespace {
    std::shared_ptr<sf::RenderWindow> g_Window;

    constexpr int TICKRATE = 66;
    constexpr double FRAME_TIME = 1.0 / TICKRATE;
}

void Application::Init(int width, int height, const std::string &title) {
    g_Window = Window::Init(width, height, title);
    g_Window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*g_Window, true);
    Window::UpdateViewport();
}

void Application::Run() {
    if (!g_Window) {
        return;
    }

    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;

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
            ImGui::SFML::ProcessEvent(event);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;
        accumulator += elapsedTime.count();

        ImGui::SFML::Update(*g_Window, deltaClock.restart());

        while (accumulator >= FRAME_TIME) {
            SceneManager::FixedUpdate();
            accumulator -= FRAME_TIME;
        }

        double deltaTime = elapsedTime.count();
        SceneManager::Update(deltaTime);

        g_Window->clear();
        LayerManager::Draw(*g_Window);
        SceneManager::Render();
        ImGui::SFML::Render(*g_Window);
        g_Window->display();
        PROFILE_END();
    }

    Application::Destroy();
}

void Application::Destroy() {
    PROFILE_BEGIN("Application Shutdown");
    LayerManager::Clear();
    SceneManager::Destroy();
    ImGui::SFML::Shutdown();
    Resources::Unload();
    Window::Destroy();
    g_Window.reset();
    PROFILE_END();
}
