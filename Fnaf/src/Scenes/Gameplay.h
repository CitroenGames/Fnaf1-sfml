#pragma once

#include "Scene/Scene.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Office.h"

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
	std::shared_ptr<sf::Music> bgaudio1;
	std::shared_ptr<sf::Music> bgaudio2;
	std::shared_ptr<sf::Music> m_FanBuzzing;

	Office m_Office;
};