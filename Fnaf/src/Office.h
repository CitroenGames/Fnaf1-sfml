#pragma once

#include "Composable.h"
#include "nlohmann/json.hpp"
#include "UI/TopBottomButtons.h"
#include "Animation/FlipBook.h"
#include "fnaf.hpp"

class Office : public Composable::Component {
public:
    Office();

    void Init();

    void Update(double deltaTime) override;

    void FixedUpdate() override;

    void Render() {
    };

    void Destroy() {
        HideOfficeElements();
        m_LeftDoor.Cleanup();
        m_RightDoor.Cleanup();
    };

    nlohmann::json Serialize() const override { return nlohmann::json(); };

    void Deserialize(const nlohmann::json &data) override {
    };

    std::string GetTypeName() const override { return "Office"; }

    void HideOfficeElements();

    void ShowOfficeElements();

    // Set game reference
    void SetGameReference(std::shared_ptr<FNAFGame> gameRef) {
        m_GameRef = gameRef;
    }

private:
    sf::Sprite m_OfficeSprite;
    FlipBook m_LeftDoor, m_RightDoor;

    std::shared_ptr<sf::Texture> m_OfficeTexture;
    std::shared_ptr<sf::Texture> m_DoorTexture;

    // Add textures for office lights
    std::shared_ptr<sf::Texture> m_LeftLightTexture;
    std::shared_ptr<sf::Texture> m_RightLightTexture;
    std::shared_ptr<sf::Texture> m_LeftLightBonnieTexture;
    std::shared_ptr<sf::Texture> m_RightLightChicaTexture;

    // Power outage textures
    std::shared_ptr<sf::Texture> m_PowerOutageTexture;   // Office_NoPower1.png (dim/dark)
    std::shared_ptr<sf::Texture> m_PowerOutageTexture2;  // Office_NoPower2.png (Freddy face)
    bool m_PowerOutage = false;
    float m_FlickerTimer = 0.0f;
    bool m_FlickerState = false;  // false=NoPower1, true=NoPower2

    ImageButton m_FreddyNoseButton;

    TopBottomButtons m_LeftButtons, m_RightButtons;

    std::shared_ptr<sf::Music> m_FreddyNose;
    std::shared_ptr<sf::Music> m_DoorNoise;
    std::shared_ptr<sf::Music> m_LightSound;
    bool m_IsVisible;

    // Game reference for power system integration
    std::shared_ptr<FNAFGame> m_GameRef;

    // Light callback methods
    void ToggleLeftLight(bool active);

    void ToggleRightLight(bool active);

    // Power outage handling
    void HandlePowerOutage();
};
