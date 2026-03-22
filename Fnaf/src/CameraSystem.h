#pragma once

#include "Animation/FlipBook.h"
#include "UI/HUDButton.h"
#include "Composable.h"
#include "UI/ImageButton.h"

class Office;

class CameraSystem : public Composable::Component {
public:
    CameraSystem();
    ~CameraSystem() = default;

    void Init();
    void Update(double deltaTime) override;
    void FixedUpdate() override;
    void Destroy();

    void ToggleCamera();
    void SetActiveCamera(const std::string& cameraId);
    bool IsActive() const { return m_IsActive; }
    void SetOfficeComponent(std::shared_ptr<Office> office) { m_OfficeRef = office; }
    
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override {}
    std::string GetTypeName() const override { return "CameraSystem"; }
    std::string GetActiveCamera() const { return m_CurrentCamera; }

private:
    bool m_IsActive;
    bool m_IsAnimatingOpen = false;
    bool m_IsAnimatingClose = false;
    std::string m_CurrentCamera;

    // Camera views
    std::map<std::string, std::shared_ptr<sf::Sprite>> m_CameraViews;
    std::map<std::string, std::map<std::string, std::shared_ptr<sf::Sprite>>> m_CameraStateViews;

    // UI elements
    std::shared_ptr<sf::Sprite> m_CameraMapSprite;
    std::shared_ptr<sf::Sprite> m_CameraBorderSprite;
    std::map<std::string, std::shared_ptr<ImageButton>> m_CameraButtons;

    // Camera name text overlays
    std::map<std::string, std::shared_ptr<sf::Sprite>> m_CameraNameSprites;

    // Animation elements
    FlipBook m_CameraFlipAnimation;
    FlipBook m_StaticAnimation;

    // Automated camera movement
    float m_CameraShakeTimer = 0.0f;
    float m_CameraShakeIntensity = 0.0f;
    sf::Vector2f m_CameraBasePosition = {0.0f, 0.0f};
    sf::Vector2f m_CameraOffset = {0.0f, 0.0f};
    std::mt19937 m_RNG{std::random_device{}()};

    void InitializeCameraViews();
    void InitializeCameraButtons();
    void InitializeAnimations();
    void InitializeCameraOverlays();
    void ShowCameraView(const std::string& cameraId);
    void ShowCameraOverlays();
    void HideCameraOverlays();
    void UpdateCameraViewBasedOnAnimatronics();
    void HideAllCameraElements();
    void UpdateCameraMovement(double deltaTime);
    
    // Office reference for syncing visibility with animation
    std::shared_ptr<Office> m_OfficeRef;

    // Sound effects
    std::shared_ptr<sf::Music> m_CameraOpenSound;
    std::shared_ptr<sf::Music> m_CameraCloseSound;
    std::shared_ptr<sf::Music> m_CameraSwitchSound;
};