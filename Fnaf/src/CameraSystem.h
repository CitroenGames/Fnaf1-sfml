#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <string>
#include <random>
#include "Animation/FlipBook.h"
#include "UI/ImageButton.h"
#include "Composable.h"
#include "LayerDefines.h"
#include <random>
#include "GameState.h"

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
    
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override {}
    std::string GetTypeName() const override { return "CameraSystem"; }

private:
    bool m_IsActive;
    std::string m_CurrentCamera;
    
    // Camera views
    std::map<std::string, std::shared_ptr<sf::Sprite>> m_CameraViews;
    std::map<std::string, std::map<std::string, std::shared_ptr<sf::Sprite>>> m_CameraStateViews;
    
    // UI elements
    std::shared_ptr<sf::Sprite> m_CameraMapSprite;
    std::shared_ptr<ImageButton> m_CameraButton;
    std::map<std::string, std::shared_ptr<ImageButton>> m_CameraButtons;
    
    // Animation elements
    FlipBook m_CameraFlipAnimation;
    FlipBook m_StaticAnimation;
    
    // Automated camera movement
    float m_CameraShakeTimer;
    float m_CameraShakeIntensity;
    sf::Vector2f m_CameraBasePosition;
    sf::Vector2f m_CameraOffset;
    std::mt19937 m_RNG;
    
    void InitializeCameraViews();
    void InitializeCameraButtons();
    void InitializeAnimations();
    void ShowCameraView(const std::string& cameraId);
    void UpdateCameraViewBasedOnAnimatronics();
    void HideAllCameraElements();
    void UpdateCameraMovement(double deltaTime);
    
    // Sound effects
    std::shared_ptr<sf::Music> m_CameraOpenSound;
    std::shared_ptr<sf::Music> m_CameraCloseSound;
    std::shared_ptr<sf::Music> m_CameraSwitchSound;
};