#pragma once

#include "Scene/Scene.h"
#include "sfml/Graphics.hpp"
#include "sfml/Audio.hpp"

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
	float scrollOffset = 0.0f; // Initial scroll offset
	float lookAngle = 0.0f; // Initial look angle

	// Create sprites
	sf::Sprite officeSprite;
	sf::Sprite leftButtonSprite;
	sf::Sprite rightButtonSprite;
	sf::Sprite leftDoorSprite;
	sf::Sprite rightDoorSprite;

	sf::Texture officeTexture;
	sf::Texture buttonTexture;
	sf::Texture doorTexture;

	sf::Music bgaudio1;
	sf::Music bgaudio2;
};