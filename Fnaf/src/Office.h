#pragma once

#include "SFML/Graphics.hpp"
#include "Composable.h"
#include "nlohmann/json.hpp"
#include "UI/TopBottomButtons.h"
#include "Animation/Flipbook.h"

class Office : public Composable::Component
{
public:
	Office();
	void Init();
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render();
	void Destroy();

	nlohmann::json Serialize()const override { return nlohmann::json(); };
	void Deserialize(const nlohmann::json& data) override {};

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
};