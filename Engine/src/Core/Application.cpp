#include "Window.h"
#include "Application.h"
#include <chrono>
#include "Scene/SceneManager.h"
#include "Layers/LayerManager.h"
#include <thread>

sf::RenderWindow* Application::window = nullptr;

const int TICKRATE = 66;  // Desired tickrate (ticks per second)
const double FRAME_TIME = 1.0 / TICKRATE;  // Time per tick (seconds)

void Application::Init()
{
	window = Window::Init(800, 600, "Window");
}

void Application::Run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    bool hasFocus = true;

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
            else if (event.type == sf::Event::LostFocus) {
                hasFocus = false;
            }
            else if (event.type == sf::Event::GainedFocus) {
                hasFocus = true;
            }
        }

        if (hasFocus) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedTime = currentTime - previousTime;
            previousTime = currentTime;
            accumulator += elapsedTime.count();

            while (accumulator >= FRAME_TIME) {
                SceneManager::FixedUpdate();
                accumulator -= FRAME_TIME;
            }

            double deltaTime = elapsedTime.count();
            SceneManager::Update(deltaTime);

            window->clear();
            SceneManager::Render();
            LayerManager::Draw(*window);
            window->display();
        }
        else {
            // sleep to keep the cpu usage low
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }
}

void Application::Destroy()
{
	Window::Destroy();
}
