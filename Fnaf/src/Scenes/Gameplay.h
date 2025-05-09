#pragma once

#include "Scene/Scene.h"
#include "Components/Camera.h"
#include "CameraSystem.h"
#include "Office.h"
#include "fnaf.hpp"

constexpr float scrollspeed = 10.0f;

class Gameplay : public Scene
{
public:
    void Init() override;
    void FixedUpdate() override;
    void Update(double deltaTime) override;
    void Render() override;
    void Destroy() override;

private:
    std::shared_ptr<sf::Music> bgaudio1;
    std::shared_ptr<sf::Music> bgaudio2;

    //TODO: MOVE THIS TO OFFICE
    std::shared_ptr<sf::Music> m_FanBuzzing;

    float scrollOffset = 175.0f; // Initial scroll offset

    Office m_Office;
    std::unique_ptr<Camera2D> m_Camera;
    
    std::shared_ptr<CameraSystem> m_CameraSystem;
    std::shared_ptr<HUDButton> m_CameraButton;

    std::shared_ptr<FNAFGame> gameplay;

	std::shared_ptr<Office> m_OfficeComponent;
    bool m_WasCameraActive = false;
};