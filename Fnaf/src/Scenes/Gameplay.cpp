#include "Gameplay.h"
#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Power.h"
#include "imgui.h"

void Gameplay::Init()
{
    auto entity = CreateEntity("Office stuff");
    entity->AddComponent<PowerIndicator>()->Init();
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
    config.resolution = sf::Vector2f(1024.0f, 576.0f);
    config.initialZoom = 1.0f;
    config.smoothingFactor = 0.75f;
    config.maintainResolution = true;

    m_Camera = std::make_unique<Camera2D>(config);
}

void Gameplay::FixedUpdate()
{
    Scene::FixedUpdate();
    
    m_Camera->shake(0.1f, 0.1f);
}

void Gameplay::Update(double deltaTime)
{
    Scene::Update(deltaTime);
    auto window = Window::GetWindow();

    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();

    // Screen scrolling based on mouse position
    const float scrollThreshold = 50.0f;
    const float maxSpeed = 500.0f;
    const float minSpeed = 100.0f;
    const float deadZoneSize = 200.0f; // Size of the deadzone in pixels

    // Calculate center and edges of the deadzone
    float screenCenterX = windowSize.x / 2.0f;
    float leftDeadZone = screenCenterX - deadZoneSize / 2;
    float rightDeadZone = screenCenterX + deadZoneSize / 2;

    float scrollSpeed = 0.0f;

    // Left side scrolling
    if (mousePos.x < leftDeadZone) {
        if (mousePos.x < scrollThreshold) {
            // At edge, maximum speed
            scrollSpeed = -maxSpeed;
        }
        else {
            // Scale speed based on position between edge and deadzone
            float speedFactor = (leftDeadZone - mousePos.x) / (leftDeadZone - scrollThreshold);
            scrollSpeed = -(minSpeed + (maxSpeed - minSpeed) * speedFactor);
        }
    }
    // Right side scrolling
    else if (mousePos.x > rightDeadZone) {
        if (mousePos.x > windowSize.x - scrollThreshold) {
            // At edge, maximum speed
            scrollSpeed = maxSpeed;
        }
        else {
            // Scale speed based on position between deadzone and edge
            float speedFactor = (mousePos.x - rightDeadZone) / ((windowSize.x - scrollThreshold) - rightDeadZone);
            scrollSpeed = minSpeed + (maxSpeed - minSpeed) * speedFactor;
        }
    }

    scrollOffset += scrollSpeed * deltaTime;

    // Clamp the scroll offset to half the viewport width
    scrollOffset = std::clamp(scrollOffset, 0.0f, 360.0f);  // 360 = 720/2

    // Calculate new camera position using viewport dimensions
    sf::Vector2f newCameraPos(scrollOffset + (VIEWPORT_WIDTH / 1.9f), (VIEWPORT_HEIGHT / 2));
    m_Camera->setPosition(newCameraPos);
    m_Camera->update(deltaTime);
    m_Camera->applyTo(*window);
}

void Gameplay::Render()
{
    ImGui::Begin("Gameplay");
	ImGui::Text("Gameplay");
	ImGui::End();
}

void Gameplay::Destroy()
{
    m_Office.Destroy();
    Scene::Destroy();
}
