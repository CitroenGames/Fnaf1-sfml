#include "Gameplay.h"
#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "imgui.h"
#include <math.h>
#include "Scenes/Menu.h"
#include "GameState.h"
#include "Scene/SceneManager.h"
#include "CameraSystem.h"

void Gameplay::Init()
{
    gameplay = std::make_unique<FNAFGame>();
    gameplay->InitializeGame(player.m_Night);

    if (player.m_Night >= 7) // Custom night
    {
        //gameplay->InitializeCustomNight(CustomAILevels);
    }

    auto entity = CreateEntity("Office stuff");
    entity->AddComponent<Office>()->Init();
    auto officeComponent = entity->GetComponent<Office>();
    
    // Add camera system
    auto cameraEntity = CreateEntity("Camera System");
    cameraEntity->AddComponent<CameraSystem>()->Init();
    m_CameraSystem = cameraEntity->GetComponent<CameraSystem>();
    
    // Load music
    bgaudio1 = Resources::GetMusic("Audio/Ambience/ambience2.wav");
    bgaudio2 = Resources::GetMusic("Audio/Ambience/EerieAmbienceLargeSca_MV005.wav");
    m_FanBuzzing = Resources::GetMusic("Audio/Office/Buzz_Fan_Florescent2.wav");

    bgaudio1->setLoop(true);
    bgaudio1->play();
    bgaudio1->setVolume(100.f);

    bgaudio2->setLoop(true);
    bgaudio2->play();
    bgaudio2->setVolume(50.f);

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
    
    // Get office component reference if we don't have it
    if (!m_OfficeComponent) {
        auto officeEntity = GetEntityByName("Office stuff");
        if (officeEntity) {
            m_OfficeComponent = officeEntity->GetComponent<Office>();
        }
    }
    
    // Handle camera toggling
    if (m_CameraSystem->IsActive() != m_WasCameraActive) {
        m_WasCameraActive = m_CameraSystem->IsActive();
        
        // Update office visibility based on camera state
        if (m_OfficeComponent) {
            if (m_CameraSystem->IsActive()) {
                m_OfficeComponent->HideOfficeElements();
            } else {
                m_OfficeComponent->ShowOfficeElements();
            }
        }
    }
}

constexpr float m_OfficeWidth = 1600.0f;
constexpr float m_ViewportWidth = 1280.0f;

void Gameplay::Update(double deltaTime)
{
    Scene::Update(deltaTime);

    gameplay->Update(deltaTime);

    if (gameplay->IsGameOver()) {
        SceneManager::QueueSwitchScene(std::make_shared<Menu>());
    }

    auto window = Window::GetWindow();
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();

    // Only handle office panning when the camera is NOT active
    if (!player.m_UsingCamera) {
        // Scrolling parameters
        const float scrollThreshold = 400.0f;
        const float maxScrollSpeed = 500.0f;
        float scrollSpeed = 0.0f;

        // Left edge scrolling with dynamic speed
        if (mousePos.x < scrollThreshold) {
            // Calculate how far the mouse is from the edge (0 to scrollThreshold)
            float distanceFromEdge = static_cast<float>(mousePos.x);
            // Convert to a percentage (1.0 at edge, 0.0 at threshold)
            float speedFactor = 1.0f - (distanceFromEdge / scrollThreshold);
            // Apply exponential curve for more natural acceleration
            speedFactor = speedFactor * speedFactor;
            // Apply to max speed
            scrollSpeed = -maxScrollSpeed * speedFactor;
        }
        // Right edge scrolling with dynamic speed
        else if (mousePos.x > (windowSize.x - scrollThreshold)) {
            // Calculate how far the mouse is from the edge (0 to scrollThreshold)
            float distanceFromEdge = static_cast<float>(windowSize.x - mousePos.x);
            // Convert to a percentage (1.0 at edge, 0.0 at threshold)
            float speedFactor = 1.0f - (distanceFromEdge / scrollThreshold);
            // Apply exponential curve for more natural acceleration
            speedFactor = speedFactor * speedFactor;
            // Apply to max speed
            scrollSpeed = maxScrollSpeed * speedFactor;
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
    }

    // Always update the camera (needed for animations and transitions)
    m_Camera->update(deltaTime);
    
    // Apply camera to the render window for game elements
    m_Camera->applyTo(*window);
    
    // After camera is applied to game view, restore default view for UI/HUD elements that shouldn't move
    if (player.m_UsingCamera) {
        // For HUD elements that should stay fixed, switch back to default view
        window->setView(window->getDefaultView());
    }
}

void Gameplay::Render()
{
    ImGui::Begin("PlayerInfo");
    {
        ImGui::Text("Night: %d", player.m_Night);
        ImGui::Text("Time: %d AM", (player.m_Time == 0) ? 12 : player.m_Time);
        ImGui::Text("Power Usage: %d", player.m_UsageLevel);
        ImGui::Text("Power: %d%%", static_cast<int>(player.m_PowerLevel));
    }
    ImGui::End();
}

void Gameplay::Destroy()
{
    if (m_CameraSystem) {
        m_CameraSystem->Destroy();
    }
    
    m_Office.Destroy();
    Scene::Destroy();
}