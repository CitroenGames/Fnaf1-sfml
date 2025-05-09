#pragma once

#include "Composable.h"
#include "nlohmann/json.hpp"
#include "UI/TopBottomButtons.h"
#include "Animation/FlipBook.h"
#include "fnaf.hpp"

class Office : public Composable::Component
{
public:
    Office();
    void Init();
    void Update(double deltaTime) override;
    void FixedUpdate() override;
    void Render() {};
    void Destroy() {};

    nlohmann::json Serialize()const override { return nlohmann::json(); };
    void Deserialize(const nlohmann::json& data) override {};

    std::string GetTypeName() const override { return "Office"; }

    void HideOfficeElements();
    void ShowOfficeElements();

    // New method to set game reference
    void SetGameReference(std::shared_ptr<FNAFGame> gameRef) {
        m_GameRef = gameRef;
    }

private:
    sf::Sprite m_OfficeSprite;
    FlipBook m_LeftDoor, m_RightDoor;

    std::shared_ptr<sf::Texture> m_OfficeTexture;
    std::shared_ptr<sf::Texture> m_LeftButtonTexture;
    std::shared_ptr<sf::Texture> m_DoorTexture;

    // Add textures for office lights
    std::shared_ptr<sf::Texture> m_LeftLightTexture;
    std::shared_ptr<sf::Texture> m_RightLightTexture;
    std::shared_ptr<sf::Texture> m_LeftLightBonnieTexture;
    std::shared_ptr<sf::Texture> m_RightLightChicaTexture;

    // Power outage textures
    std::shared_ptr<sf::Texture> m_PowerOutageTexture;

    ImageButton m_FreddyNoseButton;

    TopBottomButtons m_LeftButtons, m_RightButtons;

    FlipBook m_FanFlipBook;

    std::shared_ptr<sf::Music> m_FreddyNose;
    std::shared_ptr<sf::Music> m_DoorNoise;
    std::shared_ptr<sf::Music> m_LightSound;
    bool m_IsVisible;

    // Light state flags
    bool m_LeftLightOn;
    bool m_RightLightOn;

    // Game reference for power system integration
    std::shared_ptr<FNAFGame> m_GameRef;

    // Light callback methods
    void ToggleLeftLight(bool active);
    void ToggleRightLight(bool active);

    // Power outage handling
    void HandlePowerOutage();
};