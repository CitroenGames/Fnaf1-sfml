#pragma once

#include "Scene/Scene.h"
#include "sfml/Audio.hpp"
#include "animation/Flipbook.h"
#include "UI/ImageButton.h"
#include <memory>

class Menu : public Scene
{
public:
	void Init() override;
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render() override;
	void Destroy() override;
	void SwitchToGameplay();

private:
	std::shared_ptr<sf::Music> bgaudio2;
	std::shared_ptr<sf::Texture> m_Logo;
	sf::Sprite m_LogoSprite;
	Flipbook flipbook;
	ImageButton button;
};