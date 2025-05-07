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

constexpr float m_OfficeWidth = 1600.0f;
constexpr float m_ViewportWidth = 1280.0f;

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

    gameplay->SetCameraSystem(m_CameraSystem);

    // HUD STUFF
    {
        // Initialize camera button (to toggle camera view) as a HUD element
        m_CameraButton = std::make_shared<HUDButton>();
        m_CameraButton->SetTexture("Graphics/CameraSystem/CameraButton.png");
        m_CameraButton->SetPosition(640.0f, 650.0f);
    }

    // Load music
    {
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
    }

    // Load sound effects? 
    {
        // I have no idea where else I would put it but putting in scene is also a terrible idea in terms of keeping the code clean in the init func so stay tuned to see where I'll end up putting it :D
    }

	{ // Camera stuff
        Camera2D::Config config;
        config.resolution = sf::Vector2f(1280.0f, 720.0f);
        config.initialZoom = 1.0f;
        config.smoothingFactor = 0.75f;
        config.maintainResolution = true;

        m_Camera = std::make_unique<Camera2D>(config);
    }

}

void Gameplay::FixedUpdate()
{
    Scene::FixedUpdate();

	auto window = Window::GetWindow(); // We REALLY should get rid of this shitty mess but who cares for now

    // Check for main camera button press
    if (m_CameraButton->IsClicked(*window)) {
        m_CameraSystem->ToggleCamera();
    }
}

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

        // Check if mouse is actually within the window
        bool mouseInWindow = (mousePos.x >= 0 && mousePos.x < static_cast<int>(windowSize.x) &&
            mousePos.y >= 0 && mousePos.y < static_cast<int>(windowSize.y));

        if (mouseInWindow) {
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
            (720.0f / 2.0f)                           // Center vertically
        );
        m_Camera->setPosition(newCameraPos);
    }

    // Always update the camera (needed for animations and transitions)
    m_Camera->update(deltaTime);

    // Apply camera to the render window for game elements
    m_Camera->applyTo(*window);
}

void Gameplay::Render()
{
    auto window = Window::GetWindow();

    // Now draw HUD elements in screen space
    if (m_CameraButton) {
        m_CameraButton->Draw(*window);
    }

#if defined(_DEBUG)
    // Render UI debug information
    ImGui::Begin("PlayerInfo");
    {
        ImGui::Text("Night: %d", player.m_Night);
        ImGui::Text("Time: %d AM", (player.m_Time == 0) ? 12 : player.m_Time);
        ImGui::Text("Power Usage: %d", player.m_UsageLevel);
        ImGui::Text("Power: %d%%", static_cast<int>(player.m_PowerLevel));
    }
    ImGui::End();

    // Add new debug window for animatronic AI information
    ImGui::Begin("Animatronics AI Debug");
    {
        if (gameplay) {
            // Helper function to convert Room enum to string
            auto RoomToString = [](Room room) -> std::string {
                switch (room) {
                case Room::SHOW_STAGE: return "Show Stage";
                case Room::DINING_AREA: return "Dining Area";
                case Room::PIRATE_COVE: return "Pirate Cove";
                case Room::WEST_HALL: return "West Hall";
                case Room::EAST_HALL: return "East Hall";
                case Room::WEST_CORNER: return "West Hall Corner";
                case Room::EAST_CORNER: return "East Hall Corner";
                case Room::SUPPLY_CLOSET: return "Supply Closet";
                case Room::KITCHEN: return "Kitchen";
                case Room::RESTROOMS: return "Restrooms";
                case Room::OFFICE: return "Office";
                default: return "Unknown";
                }
                };

            std::vector<std::string> characters = { "Freddy", "Bonnie", "Chica", "Foxy" };

            for (const auto& character : characters) {
                if (ImGui::CollapsingHeader(character.c_str())) {
                    // Get animatronic information
                    Room location = gameplay->GetAnimatronicLocation(character);
                    float progress = gameplay->GetAnimatronicMovementProgress(character);

                    // Display room location
                    ImGui::Text("Location: %s", RoomToString(location).c_str());

                    // Get AI info from gameplay - need to expose this in the FNAFGame class
                    int aiLevel = gameplay->GetAnimatronicAILevel(character);
                    float timeSinceLastMove = gameplay->GetAnimatronicTimeSinceLastMove(character);
                    float moveInterval = gameplay->GetAnimatronicMoveInterval(character);

                    ImGui::Text("AI Level: %d / %d", aiLevel, MAX_AI_LEVEL);
                    ImGui::Text("Movement Progress: %.1f%%", progress);
                    ImGui::Text("Move Timer: %.2fs / %.2fs", timeSinceLastMove, moveInterval);

                    // Add progress bar for move timer
                    float timerRatio = timeSinceLastMove / moveInterval;
                    ImGui::ProgressBar(timerRatio, ImVec2(-1, 0),
                        ("Move Chance: " + std::to_string(int(100 * std::min(1.0f,
                            float(aiLevel) / float(MAX_RANDOM_ROLL)))) + "%").c_str());

                    // Add a visual indicator for movement probability
                    ImGui::Text("Movement Roll: AI Level (%d) vs Random(1-20)", aiLevel);

                    // Character-specific info
                    if (character == "Freddy") {
                        ImGui::Text("Special: Stalled by camera, moves in dark");
                    }
                    else if (character == "Bonnie") {
                        ImGui::Text("Special: Approaches from left side");
                    }
                    else if (character == "Chica") {
                        ImGui::Text("Special: Approaches from right side, makes kitchen noises");
                    }
                    else if (character == "Foxy") {
                        ImGui::Text("Special: Simplified behavior (original complex AI commented out)");
                    }
                }
            }
        }
        else {
            ImGui::Text("Gameplay not initialized!");
        }
    }
    ImGui::End();
#endif
}

void Gameplay::Destroy()
{
    if (m_CameraSystem) {
        m_CameraSystem->Destroy();
    }
    
    m_Office.Destroy();
    Scene::Destroy();
}