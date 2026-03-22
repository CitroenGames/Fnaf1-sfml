#include "CameraSystem.h"
#include "Office.h"
#include "Assets/Resources.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "LayerDefines.h"
#include "GameState.h"

CameraSystem::CameraSystem()
    : m_IsActive(false)
    , m_CurrentCamera("1A")
{
}

void CameraSystem::Init()
{
    // Load camera map - positioned in bottom-right like the real game
    m_CameraMapSprite = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CameraMap.png"));
    m_CameraMapSprite->setPosition(830.0f, 450.0f);
    m_CameraMapSprite->setOrigin(m_CameraMapSprite->getGlobalBounds().width / 2,
        m_CameraMapSprite->getGlobalBounds().height / 2);

    // Initialize camera views, buttons, animations, and overlays
    InitializeCameraViews();
    InitializeCameraButtons();
    InitializeAnimations();
    InitializeCameraOverlays();

    // Load sound effects
    m_CameraOpenSound = Resources::GetMusic("Audio/CameraSystem/Camera_Open.wav");
    m_CameraCloseSound = Resources::GetMusic("Audio/CameraSystem/Camera_Close.wav");
    m_CameraSwitchSound = Resources::GetMusic("Audio/CameraSystem/Camera_Switch.wav");

    // Start with everything hidden except the camera button - which is always visible
    HideAllCameraElements();
}

void CameraSystem::InitializeCameraViews()
{
    // Initialize default camera views
    m_CameraViews["1A"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/Normal.png"));
    m_CameraViews["1B"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/Normal.png"));
    m_CameraViews["1C"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1C/Normal.png"));
    m_CameraViews["2A"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam2A/Normal1.png"));
    m_CameraViews["2B"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam2B/Normal.png"));
    m_CameraViews["3"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam3/Normal.png"));
    m_CameraViews["4A"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam4A/Normal.png"));
    m_CameraViews["4B"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam4B/Normal.png"));
    m_CameraViews["5"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam5/Normal.png"));
    m_CameraViews["6"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam6/Normal.png"));
    m_CameraViews["7"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam7/Normal.png"));

    // Center all camera views
    for (auto& [id, sprite] : m_CameraViews) {
        sprite->setPosition(640.0f, 360.0f);
        sprite->setOrigin(sprite->getGlobalBounds().width / 2,
            sprite->getGlobalBounds().height / 2);
    }

    // Initialize alternative states for camera views (for animatronics)

    // Cam1A states
    m_CameraStateViews["1A"]["Normal"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/Normal.png"));
    m_CameraStateViews["1A"]["AllLooking"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/AllLooking.png"));
    m_CameraStateViews["1A"]["ChicaGone"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/ChicaGone.png"));
    m_CameraStateViews["1A"]["Empty"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/Empty.png")); // should we make this the normal and not the other way around?
    m_CameraStateViews["1A"]["FreddyOnly"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/FreddyOnly.png"));
    m_CameraStateViews["1A"]["FreddyOnlyLooking"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/FreddyOnlyLooking.png"));
    m_CameraStateViews["1A"]["NoBonnie"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1A/NoBonnie.png"));

    // Cam1B states
    m_CameraStateViews["1B"]["Normal"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/Normal.png"));
    m_CameraStateViews["1B"]["BonnieStage1"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/BonnieStage1.png"));
    m_CameraStateViews["1B"]["BonnieStage2"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/BonnieStage2.png"));
    m_CameraStateViews["1B"]["ChicaStage1"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/ChicaStage1.png"));
    m_CameraStateViews["1B"]["ChicaStage2"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/ChicaStage2.png"));
    m_CameraStateViews["1B"]["Freddy"] = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CamViews/Cam1B/Freddy.png"));

    // Center all camera state views
    for (auto& [camId, states] : m_CameraStateViews) {
        for (auto& [stateId, sprite] : states) {
            sprite->setPosition(640.0f, 360.0f);
            sprite->setOrigin(sprite->getGlobalBounds().width / 2,
                sprite->getGlobalBounds().height / 2);
        }
    }

    // TODO: we need the put all of the camera states ;-; this is going to be hell maybe we should use scripting language for this...
}

void CameraSystem::InitializeCameraButtons()
{
    // Create camera selection buttons
    // Map is 400x400, centered at (830, 450) → top-left at (630, 250)
    // Room centers measured from map image pixels, converted to screen coords:
    //   screen = map_pixel + (630, 250), then offset (-15, -12) for button centering (31x25px)

    // 1A - Show Stage: map center ~(200, 60) → screen (830, 310) → button (815, 298)
    m_CameraButtons["1A"] = std::make_shared<ImageButton>();
    m_CameraButtons["1A"]->SetTexture("Graphics/CameraSystem/Cam1AButton.png");
    m_CameraButtons["1A"]->SetPosition(815.0f, 298.0f);

    // 1B - Dining Area: map center ~(200, 130) → screen (830, 380) → button (815, 368)
    m_CameraButtons["1B"] = std::make_shared<ImageButton>();
    m_CameraButtons["1B"]->SetTexture("Graphics/CameraSystem/Cam1BButton.png");
    m_CameraButtons["1B"]->SetPosition(815.0f, 368.0f);

    // 1C - Pirate Cove: map center ~(45, 95) → screen (675, 345) → button (660, 333)
    m_CameraButtons["1C"] = std::make_shared<ImageButton>();
    m_CameraButtons["1C"]->SetTexture("Graphics/CameraSystem/Cam1CButton.png");
    m_CameraButtons["1C"]->SetPosition(660.0f, 333.0f);

    // 2A - West Hall: map center ~(75, 280) → screen (705, 530) → button (690, 518)
    m_CameraButtons["2A"] = std::make_shared<ImageButton>();
    m_CameraButtons["2A"]->SetTexture("Graphics/CameraSystem/Cam2AButton.png");
    m_CameraButtons["2A"]->SetPosition(690.0f, 518.0f);

    // 2B - West Hall Corner: map center ~(125, 325) → screen (755, 575) → button (740, 563)
    m_CameraButtons["2B"] = std::make_shared<ImageButton>();
    m_CameraButtons["2B"]->SetTexture("Graphics/CameraSystem/Cam2BButton.png");
    m_CameraButtons["2B"]->SetPosition(740.0f, 563.0f);

    // 3 - Supply Closet: map center ~(45, 180) → screen (675, 430) → button (660, 418)
    m_CameraButtons["3"] = std::make_shared<ImageButton>();
    m_CameraButtons["3"]->SetTexture("Graphics/CameraSystem/Cam3Button.png");
    m_CameraButtons["3"]->SetPosition(660.0f, 418.0f);

    // 4A - East Hall: map center ~(240, 290) → screen (870, 540) → button (855, 528)
    m_CameraButtons["4A"] = std::make_shared<ImageButton>();
    m_CameraButtons["4A"]->SetTexture("Graphics/CameraSystem/Cam4AButton.png");
    m_CameraButtons["4A"]->SetPosition(855.0f, 528.0f);

    // 4B - East Hall Corner: map center ~(255, 325) → screen (885, 575) → button (870, 563)
    m_CameraButtons["4B"] = std::make_shared<ImageButton>();
    m_CameraButtons["4B"]->SetTexture("Graphics/CameraSystem/Cam4BButton.png");
    m_CameraButtons["4B"]->SetPosition(870.0f, 563.0f);

    // 5 - Backstage: map center ~(330, 115) → screen (960, 365) → button (945, 353)
    m_CameraButtons["5"] = std::make_shared<ImageButton>();
    m_CameraButtons["5"]->SetTexture("Graphics/CameraSystem/Cam5Button.png");
    m_CameraButtons["5"]->SetPosition(945.0f, 353.0f);

    // 6 - Restrooms: map center ~(330, 240) → screen (960, 490) → button (945, 478)
    m_CameraButtons["6"] = std::make_shared<ImageButton>();
    m_CameraButtons["6"]->SetTexture("Graphics/CameraSystem/Cam6Button.png");
    m_CameraButtons["6"]->SetPosition(945.0f, 478.0f);

    // 7 - Kitchen: map center ~(330, 155) → screen (960, 405) → button (945, 393)
    m_CameraButtons["7"] = std::make_shared<ImageButton>();
    m_CameraButtons["7"]->SetTexture("Graphics/CameraSystem/Cam7Button.png");
    m_CameraButtons["7"]->SetPosition(945.0f, 393.0f);

    // Set layer for all buttons but don't add them to the layer manager yet
    for (auto& [id, button] : m_CameraButtons) {
        button->SetLayer(CAMERA_BUTTONS);
        // Initially, all buttons are hidden
        LayerManager::RemoveDrawable(button.get());
    }
}

void CameraSystem::InitializeAnimations()
{
    // Camera flip animation
    m_CameraFlipAnimation = FlipBook(CAMERA_ANIM_LAYER, 0.05f, false);
    for (int i = 1; i <= 11; i++) {
        m_CameraFlipAnimation.AddFrame(
            Resources::GetTexture("Graphics/CameraSystem/CameraFlipAnimation/Frame" + std::to_string(i) + ".png")
        );
    }
    m_CameraFlipAnimation.SetPosition(640.0f, 360.0f);

    // Static noise animation
    m_StaticAnimation = FlipBook(CAMERA_ANIM_LAYER, 0.05f, true);
    for (int i = 1; i <= 8; i++) {
        m_StaticAnimation.AddFrame(
            Resources::GetTexture("Graphics/Static/Noise" + std::to_string(i) + ".png")
        );
    }
    m_StaticAnimation.SetPosition(640.0f, 360.0f);
}

void CameraSystem::InitializeCameraOverlays()
{
    // Load camera border overlay
    m_CameraBorderSprite = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CameraBorder.png"));
    m_CameraBorderSprite->setPosition(640.0f, 360.0f);
    m_CameraBorderSprite->setOrigin(m_CameraBorderSprite->getGlobalBounds().width / 2,
        m_CameraBorderSprite->getGlobalBounds().height / 2);

    // Load camera name text sprites
    const std::map<std::string, std::string> cameraNameFiles = {
        {"1A", "ShowStage"},
        {"1B", "DiningArea"},
        {"1C", "Pirate Cove"},
        {"2A", "WestHall"},
        {"2B", "WestHallCorner"},
        {"3",  "SupplyCloset"},
        {"4A", "EastHall"},
        {"4B", "EastHallCorner"},
        {"5",  "Backstage"},
        {"6",  "Restrooms"},
        {"7",  "Kitchen"}
    };

    for (const auto& [camId, filename] : cameraNameFiles) {
        auto tex = Resources::GetTexture("Graphics/CameraSystem/Text/" + filename + ".png");
        if (tex) {
            m_CameraNameSprites[camId] = std::make_shared<sf::Sprite>(*tex);
            // Position in bottom-left area of the camera view (like the real game)
            m_CameraNameSprites[camId]->setPosition(100.0f, 600.0f);
        }
    }
}

void CameraSystem::Update(double deltaTime)
{
    // Update flip animation
    m_CameraFlipAnimation.Update(deltaTime);

    // Handle open animation completion
    if (m_IsAnimatingOpen && !m_CameraFlipAnimation.IsPlaying()) {
        m_IsAnimatingOpen = false;

        // Hide office elements now that the camera tablet fully covers the screen
        if (m_OfficeRef) {
            m_OfficeRef->HideOfficeElements();
        }

        // Now show all camera elements after the flip animation finishes
        LayerManager::AddDrawable(CAMERA_MAP, m_CameraMapSprite.get());
        for (auto& [id, button] : m_CameraButtons) {
            LayerManager::AddDrawable(CAMERA_BUTTONS, button.get());
        }

        // Start static animation and show camera feed
        m_StaticAnimation.Play();
        UpdateCameraViewBasedOnAnimatronics();
        ShowCameraOverlays();
    }

    // Handle close animation completion
    if (m_IsAnimatingClose && !m_CameraFlipAnimation.IsPlaying()) {
        m_IsAnimatingClose = false;
        // Clean up the flip animation frame that's still registered
        m_CameraFlipAnimation.UnregisterFromLayerManager();
    }

    if (m_IsActive && !m_IsAnimatingOpen) {
        m_StaticAnimation.Update(deltaTime);
    }
}

void CameraSystem::FixedUpdate()
{
    const auto window = Window::GetWindow();

    // Only check for camera selection buttons if camera is active and not animating
    if (m_IsActive && !m_IsAnimatingOpen) {
        for (const auto& [id, button] : m_CameraButtons) {
            if (button->IsClicked(*window)) {
                if (m_CurrentCamera != id) {
                    m_CameraSwitchSound->play();
                    m_CurrentCamera = id;
                    ShowCameraView(id);
                }
            }
        }
    }
}

void CameraSystem::ToggleCamera()
{
    // Prevent toggling while animating
    if (m_IsAnimatingOpen || m_IsAnimatingClose) {
        return;
    }

    m_IsActive = !m_IsActive;
    player.m_UsingCamera = m_IsActive;

    // Update power usage when camera state changes
    player.UpdateUsageLevel();

    if (m_IsActive) {
        // Play camera open sound
        m_CameraOpenSound->play();

        m_IsAnimatingOpen = true;

        // Reset and play flip animation forward
        // Stop() resets to frame 0, then Play(true) starts forward playback
        m_CameraFlipAnimation.Stop();
        m_CameraFlipAnimation.Play(true);

        // Do NOT show map, buttons, feed, or overlays here.
        // They will be shown in Update() when the flip animation finishes.
    }
    else {
        // Play camera close sound
        m_CameraCloseSound->play();

        m_IsAnimatingClose = true;

        // Hide camera feed, map, buttons, static, and overlays immediately
        HideAllCameraElements();

        // Show office immediately so it's visible behind the closing tablet
        if (m_OfficeRef) {
            m_OfficeRef->ShowOfficeElements();
        }

        // Play reverse camera flip animation
        m_CameraFlipAnimation.Play(false);
    }
}

void CameraSystem::SetActiveCamera(const std::string& cameraId)
{
    if (m_CameraViews.find(cameraId) != m_CameraViews.end()) {
        m_CurrentCamera = cameraId;
        if (m_IsActive && !m_IsAnimatingOpen) {
            ShowCameraView(cameraId);
        }
    }
}

void CameraSystem::ShowCameraView(const std::string& cameraId)
{
    // Validate camera ID exists
    auto it = m_CameraViews.find(cameraId);
    if (it == m_CameraViews.end()) {
        return;
    }

    // Hide all camera views
    for (const auto& [id, sprite] : m_CameraViews) {
        LayerManager::RemoveDrawable(sprite.get());
    }

    // Also hide all state views
    for (const auto& [camId, states] : m_CameraStateViews) {
        for (const auto& [stateId, sprite] : states) {
            LayerManager::RemoveDrawable(sprite.get());
        }
    }

    // Show selected camera view
    LayerManager::AddDrawable(CAMERA_FEED_LAYER, it->second.get());

    // Update camera name text overlay
    HideCameraOverlays();
    ShowCameraOverlays();
}

void CameraSystem::ShowCameraOverlays()
{
    // Show camera border
    if (m_CameraBorderSprite) {
        LayerManager::AddDrawable(CAMERA_MAP, m_CameraBorderSprite.get());
    }

    // Show camera name text for current camera
    auto nameIt = m_CameraNameSprites.find(m_CurrentCamera);
    if (nameIt != m_CameraNameSprites.end()) {
        LayerManager::AddDrawable(CAMERA_MAP, nameIt->second.get());
    }
}

void CameraSystem::HideCameraOverlays()
{
    // Hide camera border
    if (m_CameraBorderSprite) {
        LayerManager::RemoveDrawable(m_CameraBorderSprite.get());
    }

    // Hide all camera name texts
    for (const auto& [id, sprite] : m_CameraNameSprites) {
        LayerManager::RemoveDrawable(sprite.get());
    }
}

void CameraSystem::HideAllCameraElements()
{
    // Hide camera map
    LayerManager::RemoveDrawable(m_CameraMapSprite.get());

    // Hide all camera buttons
    for (auto& [id, button] : m_CameraButtons) {
        LayerManager::RemoveDrawable(button.get());
    }

    // Hide all camera views
    for (const auto& [id, sprite] : m_CameraViews) {
        LayerManager::RemoveDrawable(sprite.get());
    }

    // Hide all camera state views
    for (const auto& [camId, states] : m_CameraStateViews) {
        for (const auto& [stateId, sprite] : states) {
            LayerManager::RemoveDrawable(sprite.get());
        }
    }

    // Hide overlays
    HideCameraOverlays();

    // Stop and remove static animation
    m_StaticAnimation.Stop();
    m_StaticAnimation.UnregisterFromLayerManager();
}

void CameraSystem::UpdateCameraViewBasedOnAnimatronics()
{
    // This would need access to the FNAFGame instance to get animatronic locations
    // For demonstration purposes, let's assume we have access to it

    // For now, just show the default view
    ShowCameraView(m_CurrentCamera);
}

void CameraSystem::Destroy()
{
    // Make sure to clean up everything when the scene is destroyed
    HideAllCameraElements();

    // Cleanup all animations
    m_CameraFlipAnimation.Cleanup();
    m_StaticAnimation.Cleanup();

    // Reset state
    m_IsActive = false;
    m_IsAnimatingOpen = false;
    m_IsAnimatingClose = false;
    player.m_UsingCamera = false;
    player.UpdateUsageLevel();
}
