#pragma once

#include "Scene/Scene.h"
#include "Components/Camera.h"
#include "CameraSystem.h"
#include "Office.h"
#include "fnaf.hpp"
#include <array>

class Gameplay : public Scene {
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

    std::unique_ptr<Camera2D> m_Camera;

    std::shared_ptr<CameraSystem> m_CameraSystem;
    std::shared_ptr<HUDButton> m_CameraButton;

    std::shared_ptr<FNAFGame> gameplay;

    std::shared_ptr<Office> m_OfficeComponent;

    // Power HUD
    std::shared_ptr<sf::Texture> m_PowerLeftTexture;
    std::shared_ptr<sf::Texture> m_UsageLabelTexture;
    std::array<std::shared_ptr<sf::Texture>, 5> m_UsageBarTextures;
    sf::Sprite m_PowerLeftSprite;
    sf::Sprite m_UsageLabelSprite;
    sf::Sprite m_UsageBarsSprite;
    std::shared_ptr<sf::Font> m_PowerFont;
    sf::Text m_PowerPercentText;
};
