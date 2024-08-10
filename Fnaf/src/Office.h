#pragma once

#include "sfml/Graphics.hpp"
#include "ECS.h"
#include "nlohmann/json.hpp"
#include "UI/TopBottomButtons.h"

class Office : public ECS::TickableComponent
{
public:
	void Init();
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render();
	void Destroy();

	nlohmann::json serialize()const override { return nlohmann::json(); };
	void deserialize(const nlohmann::json& data) override {};

	std::string getTypeName() const override { return "Office"; }

private:
	float scrollOffset = 0.0f; // Initial scroll offset

	sf::Sprite m_OfficeSprite;
	sf::Sprite m_LeftButtonSprite, m_RightButtonSprite;
	sf::Sprite m_LeftDoorSprite, m_RightDoorSprite;

	std::shared_ptr<sf::Texture> m_OfficeTexture;
	std::shared_ptr<sf::Texture> m_LeftButtonTexture;
	std::shared_ptr<sf::Texture> m_DoorTexture;

	TopBottomButtons m_LeftButtons, m_Rightbuttons;
};