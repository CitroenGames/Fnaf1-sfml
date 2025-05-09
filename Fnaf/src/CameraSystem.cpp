#include "CameraSystem.h"
#include "Assets/Resources.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"

CameraSystem::CameraSystem()
    : m_IsActive(false)
    , m_CurrentCamera("1A")
{
}

void CameraSystem::Init()
{   
    // Load camera map
    m_CameraMapSprite = std::make_shared<sf::Sprite>(*Resources::GetTexture("Graphics/CameraSystem/CameraMap.png"));
    m_CameraMapSprite->setPosition(640.0f, 360.0f);
    m_CameraMapSprite->setOrigin(m_CameraMapSprite->getGlobalBounds().width / 2, 
                                m_CameraMapSprite->getGlobalBounds().height / 2);
    
    // Initialize camera views, buttons, and animations
    InitializeCameraViews();
    InitializeCameraButtons();
    InitializeAnimations();
    
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
    m_CameraButtons["1A"] = std::make_shared<ImageButton>();
    m_CameraButtons["1A"]->SetTexture("Graphics/CameraSystem/Cam1AButton.png");
    m_CameraButtons["1A"]->SetPosition(432.0f, 261.0f);
    
    m_CameraButtons["1B"] = std::make_shared<ImageButton>();
    m_CameraButtons["1B"]->SetTexture("Graphics/CameraSystem/Cam1BButton.png");
    m_CameraButtons["1B"]->SetPosition(432.0f, 311.0f);
    
    m_CameraButtons["1C"] = std::make_shared<ImageButton>();
    m_CameraButtons["1C"]->SetTexture("Graphics/CameraSystem/Cam1CButton.png");
    m_CameraButtons["1C"]->SetPosition(432.0f, 361.0f);
    
    m_CameraButtons["2A"] = std::make_shared<ImageButton>();
    m_CameraButtons["2A"]->SetTexture("Graphics/CameraSystem/Cam2AButton.png");
    m_CameraButtons["2A"]->SetPosition(523.0f, 261.0f);
    
    m_CameraButtons["2B"] = std::make_shared<ImageButton>();
    m_CameraButtons["2B"]->SetTexture("Graphics/CameraSystem/Cam2BButton.png");
    m_CameraButtons["2B"]->SetPosition(523.0f, 311.0f);
    
    m_CameraButtons["3"] = std::make_shared<ImageButton>();
    m_CameraButtons["3"]->SetTexture("Graphics/CameraSystem/Cam3Button.png");
    m_CameraButtons["3"]->SetPosition(582.0f, 361.0f);
    
    m_CameraButtons["4A"] = std::make_shared<ImageButton>();
    m_CameraButtons["4A"]->SetTexture("Graphics/CameraSystem/Cam4AButton.png");
    m_CameraButtons["4A"]->SetPosition(701.0f, 261.0f);
    
    m_CameraButtons["4B"] = std::make_shared<ImageButton>();
    m_CameraButtons["4B"]->SetTexture("Graphics/CameraSystem/Cam4BButton.png");
    m_CameraButtons["4B"]->SetPosition(701.0f, 311.0f);
    
    m_CameraButtons["5"] = std::make_shared<ImageButton>();
    m_CameraButtons["5"]->SetTexture("Graphics/CameraSystem/Cam5Button.png");
    m_CameraButtons["5"]->SetPosition(611.0f, 461.0f);
    
    m_CameraButtons["6"] = std::make_shared<ImageButton>();
    m_CameraButtons["6"]->SetTexture("Graphics/CameraSystem/Cam6Button.png");
    m_CameraButtons["6"]->SetPosition(611.0f, 511.0f);
    
    m_CameraButtons["7"] = std::make_shared<ImageButton>();
    m_CameraButtons["7"]->SetTexture("Graphics/CameraSystem/Cam7Button.png");
    m_CameraButtons["7"]->SetPosition(701.0f, 411.0f);
    
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

void CameraSystem::Update(double deltaTime)
{
    // Update animations
    m_CameraFlipAnimation.Update(deltaTime);
    
    if (m_IsActive) {
        m_StaticAnimation.Update(deltaTime);
        
        // If the camera flip animation is done, show the camera view
        if (!m_CameraFlipAnimation.IsPlaying()) {
            // Update camera view based on animatronics positions
            UpdateCameraViewBasedOnAnimatronics();
        }
    }
}

void CameraSystem::FixedUpdate()
{
    auto window = Window::GetWindow();
    
    // Only check for camera selection buttons if camera is active
    if (m_IsActive) {
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
    m_IsActive = !m_IsActive;
    player.m_UsingCamera = m_IsActive;
    
    if (m_IsActive) {
        // Play camera open sound
        m_CameraOpenSound->play();
        
        // Show camera map
        LayerManager::AddDrawable(CAMERA_MAP, m_CameraMapSprite.get());
        
        // Register camera selection buttons
        for (auto& [id, button] : m_CameraButtons) {
            LayerManager::AddDrawable(CAMERA_BUTTONS, button.get());
        }
        
        // Play camera flip animation
        m_CameraFlipAnimation.Play();
        m_StaticAnimation.Play();
        
        // Show current camera view after animation
        ShowCameraView(m_CurrentCamera);
    }
    else {
        // Play camera close sound
        m_CameraCloseSound->play();
        
        // Hide all camera elements
        HideAllCameraElements();
        
        // Play reverse camera flip animation
        m_CameraFlipAnimation.Play(false);
    }
}

void CameraSystem::SetActiveCamera(const std::string& cameraId)
{
    if (m_CameraViews.find(cameraId) != m_CameraViews.end()) {
        m_CurrentCamera = cameraId;
        if (m_IsActive) {
            ShowCameraView(cameraId);
        }
    }
}

void CameraSystem::ShowCameraView(const std::string& cameraId)
{
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
    LayerManager::AddDrawable(CAMERA_FEED_LAYER, m_CameraViews[cameraId].get());
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
    player.m_UsingCamera = false;
}