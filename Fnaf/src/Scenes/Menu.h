#pragma once

#include "Scene/Scene.h"
#include "sfml/Audio.hpp"
#include "animation/Flipbook.h"
#include <memory>

class Menu : public Scene
{
public:
	Menu() {};
	~Menu() {};

	void Init() override;
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render() override;
	void Destroy() override;

private:
	std::shared_ptr<sf::Music> bgaudio2;
	sf::Texture texture1, texture2, texture3, texture4;
	sf::Sprite sprite1, sprite2, sprite3, sprite4;
	Flipbook flipbook;
};