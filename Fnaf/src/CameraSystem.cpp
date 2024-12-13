#include "CameraSystem.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "LayerDefines.h"
#include "GameState.h"

CameraSystem::CameraSystem()
    : m_CurrentRoom(Room::SHOW_STAGE)
    , m_PreviousRoom(Room::SHOW_STAGE)
    , m_IsActive(false)
    , m_IsTransitioning(false)
    , m_IsTogglingCamera(false)
    , m_ToggleProgress(0.0f)
    , m_TransitionProgress(0.0f)
    , m_StaticIntensity(0.0f)
    , m_StaticTimer(0.0f)
    , m_WhiteTimer(0.0f)
    , m_CurrentStaticFrame(0)
    , m_CurrentWhiteFrame(0)
{
}

CameraSystem::~CameraSystem() {
    // Clean up buttons
    m_CameraButtons.clear();

    // Remove map sprite from layer
    LayerManager::RemoveDrawable(&m_MapSprite);
}

void CameraSystem::Init() {
    // Load camera views
    m_CameraViews[Room::SHOW_STAGE].setTexture(*Resources::GetTexture("Graphics/Cameras/ShowStage.png"));
    m_CameraViews[Room::DINING_AREA].setTexture(*Resources::GetTexture("Graphics/Cameras/DiningArea.png"));
    m_CameraViews[Room::PIRATE_COVE].setTexture(*Resources::GetTexture("Graphics/Cameras/PirateCove.png"));
    m_CameraViews[Room::WEST_HALL].setTexture(*Resources::GetTexture("Graphics/Cameras/WestHall.png"));
    m_CameraViews[Room::EAST_HALL].setTexture(*Resources::GetTexture("Graphics/Cameras/44.png")); // East Hall is 44.png
    m_CameraViews[Room::WEST_CORNER].setTexture(*Resources::GetTexture("Graphics/Cameras/WestHallCorner.png"));
    m_CameraViews[Room::EAST_CORNER].setTexture(*Resources::GetTexture("Graphics/Cameras/EastHallCorner.png"));
    m_CameraViews[Room::SUPPLY_CLOSET].setTexture(*Resources::GetTexture("Graphics/Cameras/SupplyCloset.png"));

    // Initialize static effects
    m_StaticEffect.setTexture(*Resources::GetTexture("Graphics/Static/Noise1.png"));
    m_WhiteEffect.setTexture(*Resources::GetTexture("Graphics/Static/WhiteThing1.png"));

    // Set blend modes
    m_StaticEffect.setColor(sf::Color(255, 255, 255, 128)); // 50% opacity
    m_WhiteEffect.setColor(sf::Color(255, 255, 255, 64));   // 25% opacity

    // Initialize camera button
    m_CameraButton = std::make_unique<ImageButton>();
    m_CameraButton->SetTexture("Graphics/CurrentlyActiveCam/145.png");
    m_CameraButton->SetLayer(CAMERA_UI_LAYER);
    m_CameraButton->SetPosition({20.f, 550.f}); // Bottom left corner

    // Initialize camera map
    InitializeMap();
}

void CameraSystem::InitializeMap() {
    // Set up the camera map background
    m_MapSprite.setTexture(*Resources::GetTexture("Graphics/CurrentlyActiveCam/CameraMap.png"));
    m_MapSprite.setPosition(800.f, 50.f); // Position in top-right corner

    // Create camera buttons with their positions
    struct CameraButtonInfo {
        Room room;
        sf::Vector2f position;
        const char* label;
    };

    std::vector<CameraButtonInfo> buttonInfos = {
        {Room::SHOW_STAGE, {850.f, 100.f}, "1A"},
        {Room::DINING_AREA, {850.f, 200.f}, "1B"},
        {Room::PIRATE_COVE, {750.f, 150.f}, "1C"},
        {Room::WEST_HALL, {800.f, 300.f}, "2A"},
        {Room::EAST_HALL, {900.f, 300.f}, "2B"},
        {Room::WEST_CORNER, {800.f, 400.f}, "2C"},
        {Room::EAST_CORNER, {900.f, 400.f}, "2D"},
        {Room::SUPPLY_CLOSET, {750.f, 300.f}, "3"},
        {Room::KITCHEN, {950.f, 200.f}, "4"}
    };

    for (const auto& info : buttonInfos) {
        auto button = std::make_unique<ImageButton>();
        button->SetTexture(*Resources::GetTexture("Graphics/CurrentlyActiveCam/CamButton.png"));
        button->SetPosition(info.position);
        button->SetScale({0.5f, 0.5f});
        button->SetClickCallback([this, room = info.room]() {
            if (m_IsActive) {
                SwitchToRoom(room);
            }
        });
        m_CameraButtons[info.room] = std::move(button);
    }

    // Add to appropriate layer
    LayerManager::AddDrawable(m_MapSprite, CAMERA_UI_LAYER);
}

void CameraSystem::Update(float deltaTime) {
    // Update power-based static intensity
    float powerBasedStatic = 1.0f - (player.m_PowerLevel / 100.0f);
    SetStaticIntensity(powerBasedStatic);

    if (m_IsTransitioning) {
        m_TransitionProgress += deltaTime * TRANSITION_SPEED;

        // Update static effect intensity during transition
        float transitionStaticIntensity = std::min(1.0f, m_TransitionProgress * 2.0f);
        if (m_TransitionProgress > 0.5f) {
            transitionStaticIntensity = std::min(1.0f, (1.0f - m_TransitionProgress) * 2.0f);
        }
        m_StaticEffect.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(transitionStaticIntensity * 255)));

        if (m_TransitionProgress >= 1.0f) {
            m_IsTransitioning = false;
            m_TransitionProgress = 0.0f;
            // Reset static effect to power-based intensity
            UpdateStaticEffect();
        }
    }

    // Handle camera toggle animation
    if (m_IsTogglingCamera) {
        m_ToggleProgress += deltaTime * TOGGLE_SPEED;

        if (m_ToggleProgress >= 1.0f) {
            m_IsTogglingCamera = false;
            m_ToggleProgress = 0.0f;
            m_IsActive = !m_IsActive;
            player.m_UsingCamera = m_IsActive;
        }
    }

    // Update static noise animation
    m_StaticTimer += deltaTime;
    if (m_StaticTimer >= STATIC_FRAME_DURATION) {
        m_StaticTimer = 0.0f;
        m_CurrentStaticFrame = (m_CurrentStaticFrame + 1) % 8;
        m_StaticEffect.setTexture(*Resources::GetTexture("Graphics/Static/Noise" + std::to_string(m_CurrentStaticFrame + 1) + ".png"));
    }

    // Update white effect animation
    m_WhiteTimer += deltaTime;
    if (m_WhiteTimer >= WHITE_FRAME_DURATION) {
        m_WhiteTimer = 0.0f;
        m_CurrentWhiteFrame = (m_CurrentWhiteFrame + 1) % 11;
        m_WhiteEffect.setTexture(*Resources::GetTexture("Graphics/Static/WhiteThing" + std::to_string(m_CurrentWhiteFrame + 1) + ".png"));
    }
}

void CameraSystem::SwitchToRoom(Room newRoom) {
    if (newRoom == m_CurrentRoom || m_IsTransitioning) {
        return;
    }

    m_PreviousRoom = m_CurrentRoom;
    m_CurrentRoom = newRoom;
    m_IsTransitioning = true;
    m_TransitionProgress = 0.0f;

    // Play camera switch sound
    Resources::GetSound("Audio/CameraChange.wav")->play();
}

void CameraSystem::Render(sf::RenderTarget& target) {
    // Don't render camera view when inactive and not toggling
    if (!m_IsActive && !m_IsTogglingCamera) return;

    // Calculate view alpha based on toggle state
    float viewAlpha = 255.0f;
    if (m_IsTogglingCamera) {
        viewAlpha = m_IsActive ?
            (1.0f - m_ToggleProgress) * 255.0f :
            m_ToggleProgress * 255.0f;
    }

    // Apply alpha to current view
    for (auto& [room, view] : m_CameraViews) {
        view.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(viewAlpha)));
    }

    // Render current camera view
    if (!m_IsTransitioning || m_TransitionProgress > 0.5f) {
        target.draw(m_CameraViews[m_CurrentRoom]);
    } else {
        target.draw(m_CameraViews[m_PreviousRoom]);
    }

    // Render static effects with additive blending
    sf::RenderStates additiveBlend;
    additiveBlend.blendMode = sf::BlendAdd;

    // Increase static during toggle
    float staticMultiplier = m_IsTogglingCamera ? 2.0f : 1.0f;
    m_StaticEffect.setColor(sf::Color(255, 255, 255,
        static_cast<sf::Uint8>(m_StaticIntensity * 128 * staticMultiplier)));
    m_WhiteEffect.setColor(sf::Color(255, 255, 255,
        static_cast<sf::Uint8>(m_StaticIntensity * 64 * staticMultiplier)));

    target.draw(m_StaticEffect, additiveBlend);
    target.draw(m_WhiteEffect, additiveBlend);

    // Render camera UI
    auto* window = dynamic_cast<sf::RenderWindow*>(&target);
    if (window) {
        for (auto& [room, button] : m_CameraButtons) {
            // Highlight current room's button
            if (room == m_CurrentRoom) {
                button->setColor(sf::Color(255, 255, 0, 255)); // Yellow tint
            } else {
                button->setColor(sf::Color::White);
            }

            // Handle button clicks
            if (button->IsClicked(*window)) {
                SwitchToRoom(room);
            }

            button->Render(*window);
        }
    }
    m_CameraButton->Render(target);
}

void CameraSystem::UpdateStaticEffect() {
    // Calculate base static intensity from power level
    float powerBasedStatic = 1.0f - (player.m_PowerLevel / 100.0f);

    // Add extra intensity during transitions
    float transitionIntensity = m_IsTransitioning ? 0.5f : 0.0f;

    // Combine intensities and clamp
    float combinedIntensity = std::min(1.0f, powerBasedStatic + transitionIntensity);

    // Apply to both effects
    m_StaticEffect.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(combinedIntensity * 128)));
    m_WhiteEffect.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(combinedIntensity * 64)));
}

void CameraSystem::SetStaticIntensity(float intensity) {
    m_StaticIntensity = std::clamp(intensity, 0.0f, 1.0f);
    UpdateStaticEffect();
}

void CameraSystem::ToggleCamera()
{
    static bool lastSpaceState = false;
    bool currentSpaceState = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    // Only toggle on key press, not hold
    if (currentSpaceState && !lastSpaceState) {
        m_IsActive = !m_IsActive;

        // Update power system
        player.m_UsingCamera = m_IsActive;

        // Play camera toggle sound
        if (m_IsActive) {
            Resources::GetSound("Audio/CameraUp.wav")->play();
        } else {
            Resources::GetSound("Audio/CameraDown.wav")->play();
        }

        // Reset transition state when toggling camera
        m_IsTransitioning = true;
        m_TransitionProgress = 0.0f;
    }

    lastSpaceState = currentSpaceState;
}
