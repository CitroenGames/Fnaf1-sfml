#include "Window.h"
#include "Application.h"
#include <chrono>
#include "Scene/SceneManager.h"
#include "Graphics/LayerManager.h"
#include <thread>
#include "imgui/imgui-SFML.h"

sf::RenderWindow* Application::m_Window = nullptr;

const int TICKRATE = 66;  // Desired tickrate (ticks per second)
const double FRAME_TIME = 1.0 / TICKRATE;  // Time per tick (seconds)

void Application::Init()
{
	m_Window = Window::Init(1024, 576, "Five Nights at Freddy's");
    ImGui::SFML::Init(*m_Window, true);
}

void Application::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    bool hasFocus = true;
    sf::Event event;
    sf::Clock deltaClock;

    while (m_Window->isOpen()) {
        while (m_Window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // this will close the window
                return;
            }
            else if (event.type == sf::Event::LostFocus) {
                hasFocus = false;
            }
            else if (event.type == sf::Event::GainedFocus) {
                hasFocus = true;
            }
            ImGui::SFML::ProcessEvent(event);
        }

        if (hasFocus) {
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
        else {
            // Sleep to reduce CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    Application::Destroy();
}

void Application::Destroy()
{
    //TODO: Add a proper way to close the application
    SceneManager::Destroy();
    ImGui::SFML::Shutdown();
	Window::Destroy();
}
