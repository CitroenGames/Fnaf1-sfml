#pragma once

#include "Scene/Scene.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Office.h"
#include "Components/Camera.h"
#include "fnaf.hpp"
#include "CameraSystem.h" 

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

    std::unique_ptr<FNAFGame> gameplay;

	std::shared_ptr<Office> m_OfficeComponent;
    bool m_WasCameraActive = false;
};