#include "Gameplay.h"
#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Power.h"
#include "imgui.h"
#include <math.h>
#include "Scenes/Menu.h"
#include "Scene/SceneManager.h"

void Gameplay::Init()
{
    auto entity = CreateEntity("Office stuff");
    std::shared_ptr<PowerIndicator> powerIndicator = entity->AddComponent<PowerIndicator>();
    powerIndicator->OnPowerDepletedDelegate.Add(this, &Gameplay::OnPowerOut);
    powerIndicator->Init();

    entity->AddComponent<Office>()->Init();
    auto officeComponent = entity->GetComponent<Office>();
    //AddComponent(officeComponent);
    
    // Load music
    bgaudio1 = Resources::GetMusic("Audio/Ambience/ambience2.wav");
    bgaudio1->setLoop(true);
    bgaudio1->play();
    bgaudio1->setVolume(100.f);

    bgaudio2 = Resources::GetMusic("Audio/Ambience/EerieAmbienceLargeSca_MV005.wav");
    bgaudio2->setLoop(true);
    bgaudio2->play();
    bgaudio2->setVolume(50.f);

    m_FanBuzzing = Resources::GetMusic("Audio/Office/Buzz_Fan_Florescent2.wav");
	m_FanBuzzing->setLoop(true);
	m_FanBuzzing->play();
	m_FanBuzzing->setVolume(40.f);

    Camera2D::Config config;
    config.resolution = sf::Vector2f(1280.0f, 720.0f);
    config.initialZoom = 1.0f;
    config.smoothingFactor = 0.75f;
    config.maintainResolution = true;

    m_Camera = std::make_unique<Camera2D>(config);
}

void Gameplay::FixedUpdate()
{
    Scene::FixedUpdate();
}

constexpr float m_OfficeWidth = 1600.0f;
constexpr float m_ViewportWidth = 1280.0f;

void Gameplay::Update(double deltaTime)
{
    Scene::Update(deltaTime);

    auto window = Window::GetWindow();
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();

    // Scrolling parameters
    const float scrollThreshold = 50.0f;
    const float maxScrollSpeed = 500.0f;
    float scrollSpeed = 0.0f;

    // Left edge scrolling
    if (mousePos.x < scrollThreshold) {
        scrollSpeed = -maxScrollSpeed;
    }
    // Right edge scrolling
    else if (mousePos.x > (windowSize.x - scrollThreshold)) {
        scrollSpeed = maxScrollSpeed;
    }

    // Update scroll offset
    scrollOffset += scrollSpeed * deltaTime;

    // Clamp scroll offset to image bounds
    scrollOffset = std::clamp(
        scrollOffset,
        0.0f,
        std::max(0.0f, m_OfficeWidth - m_ViewportWidth)
    );

    // Calculate new camera position
    sf::Vector2f newCameraPos(
        scrollOffset + (m_ViewportWidth / 2.0f),  // Center horizontally
        (720.0f / 2.0f)                          // Center vertically
    );

    m_Camera->setPosition(newCameraPos);
    m_Camera->update(deltaTime);
    m_Camera->applyTo(*window);

    // Update the player's time every 89 seconds
    static double elapsedTime = 0.0;
    elapsedTime += deltaTime;

    if (elapsedTime >= 89.0) {
        elapsedTime -= 89.0; // Reset the timer for the next interval
        player.m_Time++;

        if (player.m_Time == 6) {
            // Game Complete
            std::cout << "Game complete!" << std::endl;
            // Additional logic for game completion can go here
        }
    }
}

void Gameplay::Render()
{
    ImGui::Begin("PlayerInfo");

    ImGui::Text("Night: %d", player.m_Night);
    ImGui::Text("Time: %d AM", (player.m_Time == 0) ? 12 : player.m_Time);
    ImGui::Text("Power Usage: %d", player.m_UsageLevel);
    ImGui::Text("Power: %d%%", static_cast<int>(player.m_PowerLevel));
    ImGui::End();
}

void Gameplay::Destroy()
{
    m_Office.Destroy();
    Scene::Destroy();
}

void Gameplay::OnPowerOut()
{
    std::cout << "Power out" << std::endl;
    Destroy();
	SceneManager::QueueSwitchScene(std::make_shared<Menu>());
}
