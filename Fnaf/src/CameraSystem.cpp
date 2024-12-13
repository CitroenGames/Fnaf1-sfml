#include "CameraSystem.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "LayerDefines.h"
#include "GameState.h"
#include "Core/Window.h"  // Updated include path

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

    // Initialize camera button animation
    InitializeCameraButton();

    // Initialize camera map
    InitializeMap();
}

void CameraSystem::InitializeMap() {
    // Set up the camera map background
    m_MapSprite.setTexture(*Resources::GetTexture("Graphics/CurrentlyActiveCam/CameraMap.png"));
    m_MapSprite.setPosition(800.f, 50.f); // Position in top-right corner

    // Create camera buttons with their positions and textures
    struct CameraButtonInfo {
        Room room;
        sf::Vector2f position;
        const char* texturePath;
    };

    std::vector<CameraButtonInfo> buttonInfos = {
        {Room::SHOW_STAGE, {850.f, 100.f}, "Graphics/CameraButtons/CAM1A.png"},
        {Room::DINING_AREA, {850.f, 200.f}, "Graphics/CameraButtons/CAM1B.png"},
        {Room::PIRATE_COVE, {750.f, 150.f}, "Graphics/CameraButtons/CAM1C.png"},
        {Room::WEST_HALL, {800.f, 300.f}, "Graphics/CameraButtons/CAM2A.png"},
        {Room::EAST_HALL, {900.f, 300.f}, "Graphics/CameraButtons/CAM2B.png"},
        {Room::SUPPLY_CLOSET, {750.f, 300.f}, "Graphics/CameraButtons/CAM3.png"},
        {Room::KITCHEN, {950.f, 200.f}, "Graphics/CameraButtons/CAM4A.png"},
        {Room::WEST_CORNER, {800.f, 400.f}, "Graphics/CameraButtons/CAM4B.png"},
        {Room::EAST_CORNER, {900.f, 400.f}, "Graphics/CameraButtons/CAM5.png"}
    };

    for (const auto& info : buttonInfos) {
        auto button = std::make_unique<ImageButton>();
        button->SetTexture(*Resources::GetTexture(info.texturePath));
        button->SetPosition(info.position);
        button->setScale({0.5f, 0.5f});
        button->setOnClick([this, room = info.room]() {
            if (m_IsActive) {
                SwitchToRoom(room);
            }
        });
        m_CameraButtons[info.room] = std::move(button);
    }

    // Add to appropriate layer
    LayerManager::AddDrawable(CAMERA_UI_LAYER, &m_MapSprite);
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

    // Update camera button animation
    if (m_CameraButtonAnimation) {
        m_CameraButtonAnimation->Update(deltaTime);
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
    // Always render camera button animation
    if (m_CameraButtonAnimation && m_CameraButtonAnimation->GetCurrentFrame()) {
        target.draw(*m_CameraButtonAnimation->GetCurrentFrame());
    }

    // Don't render camera view or UI when inactive and not toggling
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
    target.draw(m_StaticEffect, additiveBlend);
    target.draw(m_WhiteEffect, additiveBlend);

    // Only render camera UI (map and buttons) when camera is fully active
    auto* window = dynamic_cast<sf::RenderWindow*>(&target);
    if (window && m_IsActive && !m_IsTogglingCamera) {
        // Draw map background
        target.draw(m_MapSprite);

        // Draw and handle camera buttons
        for (auto& [room, button] : m_CameraButtons) {
            // Only draw and process buttons when camera is fully active
            button->setColor(room == m_CurrentRoom ?
                sf::Color(255, 255, 0, static_cast<sf::Uint8>(viewAlpha)) :  // Yellow for current room
                sf::Color(255, 255, 255, static_cast<sf::Uint8>(viewAlpha)));
            target.draw(*button);

            if (button->IsClicked(*window)) {
                SwitchToRoom(room);
            }
        }
    }
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
    static bool lastMouseState = false;
    bool currentMouseState = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    // Only toggle on mouse click (not hold) and when mouse is over button
    if (currentMouseState && !lastMouseState && IsMouseOverCameraButton(*Window::GetWindow())) {
        // Toggle camera state
        m_IsActive = !m_IsActive;
        player.m_UsingCamera = m_IsActive;

        // Play animation and sound
        if (m_IsActive) {
            m_CameraButtonAnimation->Play(true);  // Forward animation
            Resources::GetSound("Audio/CameraUp.wav")->play();
        } else {
            m_CameraButtonAnimation->Play(false); // Reverse animation
            Resources::GetSound("Audio/CameraDown.wav")->play();
        }

        m_IsTogglingCamera = true;
        m_ToggleProgress = 0.0f;
    }

    lastMouseState = currentMouseState;
}

void CameraSystem::InitializeCameraButton() {
    m_CameraButtonAnimation = std::make_unique<FlipBook>(CAMERA_BUTTON_LAYER, 0.08f, false);
    // Load camera button frames
    for (int i = 1; i <= 11; i++) {
        std::string framePath = "Graphics/Office/Camera/Frame" + std::to_string(i) + ".png";
        m_CameraButtonAnimation->AddFrame(Resources::GetTexture(framePath));
    }
    m_CameraButtonAnimation->SetPosition(512.f, 680.f);
}

bool CameraSystem::IsMouseOverCameraButton(const sf::RenderWindow& window) const {
    if (!m_CameraButtonAnimation || !m_CameraButtonAnimation->GetCurrentFrame()) return false;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    return m_CameraButtonAnimation->GetCurrentFrame()->getGlobalBounds().contains(worldPos);
}
