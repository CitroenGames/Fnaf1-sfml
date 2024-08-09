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
	sf::Sprite leftButtonSprite, rightButtonSprite;
	sf::Sprite leftDoorSprite, rightDoorSprite;

	std::shared_ptr<sf::Texture> officeTexture;
	std::shared_ptr<sf::Texture> buttonTexture;
	std::shared_ptr<sf::Texture> doorTexture;

	std::shared_ptr<sf::Music> bgaudio1;
	std::shared_ptr<sf::Music> bgaudio2;
	std::shared_ptr<sf::Music> m_FanBuzzing;
};