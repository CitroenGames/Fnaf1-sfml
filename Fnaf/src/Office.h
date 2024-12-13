#pragma once

#include "SFML/Graphics.hpp"
#include "Composable.h"
#include "nlohmann/json.hpp"
#include "UI/TopBottomButtons.h"
#include "Animation/FlipBook.h"

// Maximum power usage level
constexpr int MAX_POWER_USAGE = 5;

class Office : public Composable::Component
{
public:
	Office();
	void Init();
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render();
	void Destroy();

	// Door and Light Control Methods
	bool IsLeftDoorClosed() const { return m_LeftDoorClosed; }
	bool IsRightDoorClosed() const { return m_RightDoorClosed; }
	bool IsLeftLightOn() const { return m_LeftLightOn; }
	bool IsRightLightOn() const { return m_RightLightOn; }

	void ToggleLeftDoor();
	void ToggleRightDoor();
	void ToggleLeftLight();
	void ToggleRightLight();

	nlohmann::json Serialize() const override { return nlohmann::json(); }
	void Deserialize(const nlohmann::json& data) override { }

	std::string GetTypeName() const override { return "Office"; }

private:
	sf::Sprite m_OfficeSprite;
	FlipBook m_LeftDoor, m_RightDoor;

	std::shared_ptr<sf::Texture> m_OfficeTexture;
	std::shared_ptr<sf::Texture> m_LeftButtonTexture;
	std::shared_ptr<sf::Texture> m_DoorTexture;

	ImageButton m_FreddyNoseButton;

	TopBottomButtons m_LeftButtons, m_RightButtons;

	FlipBook m_FanFlipBook;

	std::shared_ptr<sf::Music> m_FreddyNose;
	std::shared_ptr<sf::Music> m_DoorNoise;

	// Door and light state
	bool m_LeftDoorClosed = false;
	bool m_RightDoorClosed = false;
	bool m_LeftLightOn = false;
	bool m_RightLightOn = false;

	// Helper methods
	void UpdatePowerUsage();
	void UpdateDoorStates();
	void UpdateLightStates();
};
