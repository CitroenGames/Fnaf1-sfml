#pragma once

#include "Scene/Scene.h"
#include "SFML/Audio.hpp"
#include "Animation/Flipbook.h"
#include "Animation/GlitchEffect.h"
#include "UI/ImageButton.h"
#include <memory>
#include "Graphics/FadeEffect.h"

class Menu : public Scene
{
public:
	void Init() override;
	void Update(double deltaTime) override;
	void FixedUpdate() override;
	void Render() override;
	void Destroy() override;
	void SwitchToGameplay();

	void ShowNewsPaper();

private:
	std::shared_ptr<sf::Music> m_BgStatic;
	std::shared_ptr<sf::Music> m_MenuMusic;
	std::shared_ptr<sf::Texture> m_Logo;
	std::shared_ptr<sf::Texture> NewsPaperTexture;
	
	sf::Sprite m_LogoSprite;
	sf::Sprite NewsPaperSprite;

	ImageButton newbutton;
	GlitchEffect m_FreddyGlitchEffect;
	GlitchEffect m_StaticGlitchEffect;

	enum State { FADE_IN, WAIT, FADE_OUT, DONE } m_State = FADE_IN;
	sf::Time waitTime = sf::seconds(5);
	sf::Time accumulatedTime = sf::Time::Zero;
	FadeEffect fadeEffect;

private:
	// temp holding static here
	std::shared_ptr<sf::Texture> noise1;
	std::shared_ptr<sf::Texture> noise2;
	std::shared_ptr<sf::Texture> noise3;
};