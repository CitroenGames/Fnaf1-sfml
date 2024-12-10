#include "Window.h"
#include "Application.h"
#include <chrono>
#include "Scene/SceneManager.h"
#include "Graphics/LayerManager.h"
#include <thread>
#include "imgui/imgui-SFML.h"

std::shared_ptr<sf::RenderWindow> Application::m_Window = nullptr;

constexpr int TICKRATE = 66;  // Desired tickrate (ticks per second)
constexpr double FRAME_TIME = 1.0 / TICKRATE;  // Time per tick (seconds)

void Application::Init(const int width, const int height, const std::string& title)
{
	m_Window = Window::Init(width, height, title.data());
    m_Window->setVerticalSyncEnabled(true);
    ImGui::SFML::Init(*m_Window, true);
    Window::UpdateViewport();
}

void Application::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    
    sf::Event event;
    sf::Clock deltaClock;

    while (m_Window->isOpen()) {
        while (m_Window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_Window->close();
                break;
            }
            else if (event.type == sf::Event::Resized) {
                Window::UpdateViewport();
            }
            ImGui::SFML::ProcessEvent(event);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;
        accumulator += elapsedTime.count();

        ImGui::SFML::Update(*m_Window, deltaClock.restart());

        // Fixed Update
        while (accumulator >= FRAME_TIME) {
            SceneManager::FixedUpdate();
            accumulator -= FRAME_TIME;
        }

        // Update
        double deltaTime = elapsedTime.count();
        SceneManager::Update(deltaTime);

        // Render
        m_Window->clear();
        LayerManager::Draw(*m_Window);
        SceneManager::Render();
        ImGui::SFML::Render(*m_Window);
        m_Window->display();
    }

    Application::Destroy();
}

void Application::Destroy()
{
    //TODO: Add a proper way to close the application
    LayerManager::Clear();
    SceneManager::Destroy();
    ImGui::SFML::Shutdown();
	Window::Destroy();
}
