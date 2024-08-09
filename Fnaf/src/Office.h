#pragma once

#include "sfml/Graphics.hpp"
#include "ECS.h"
#include "nlohmann/json.hpp"

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
	float lookAngle = 0.0f; // Initial look angle

	sf::Sprite officeSprite;
	sf::Sprite leftButtonSprite, rightButtonSprite;
	sf::Sprite leftDoorSprite, rightDoorSprite;

	std::shared_ptr<sf::Texture> officeTexture;
	std::shared_ptr<sf::Texture> buttonTexture;
	std::shared_ptr<sf::Texture> doorTexture;
};