#pragma once

#include "Scene/Scene.h"
#include "sfml/Graphics.hpp"
#include "sfml/Audio.hpp"
#include "office.h"

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