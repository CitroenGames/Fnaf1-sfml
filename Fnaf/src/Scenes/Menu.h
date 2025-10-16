#pragma once

#include "Scene/Scene.h"
#include "Animation/GlitchEffect.h"
#include "UI/ImageButton.h"
#include "Graphics/FadeEffect.h"

// Audio keys - store just the identifiers
#define STATIC_AUDIO_KEY "Audio/static2.wav"
#define MENU_MUSIC_KEY "Audio/Menu/darknessmusic.wav"

class Menu : public Scene
{
public:
    Menu();
    void Init() override;
    void Update(double deltaTime) override;
    void FixedUpdate() override;
    void Render() override {}
    void Destroy() override {}
    void SwitchToGameplay();

    void ShowNewsPaper();
    void HideAllMenuElements();
    void ShowMainMenuElements();

    // Methods to handle glitch effects
    void HideGlitchEffects();
    void ShowGlitchEffects();



private:
    // Textures
    std::shared_ptr<sf::Texture> m_Logo;
    std::shared_ptr<sf::Texture> NewsPaperTexture;
    std::shared_ptr<sf::Texture> m_WarningMessageTexture;
    std::shared_ptr<sf::Texture> m_LoadingScreenTexture;

    sf::Sprite m_LogoSprite;
    sf::Sprite NewsPaperSprite;
    sf::Sprite m_WarningMessageSprite;
    sf::Sprite m_LoadingScreenSprite;
    sf::Text m_TimeText;
    sf::Text m_NightText;

    ImageButton newbutton;
    GlitchEffect m_FreddyGlitchEffect;
    GlitchEffect m_StaticGlitchEffect;
    GlitchEffect m_WhiteGlitchEffect;

    enum GameplayTransitionState {
        MAIN_MENU,
        FADE_IN,
        NEWSPAPER,
        FADE_OUT,
        TIME_DISPLAY,
        LOADING_SCREEN,
        COMPLETE
    } m_GameplayTransitionState = MAIN_MENU;

    inline void SwitchState(GameplayTransitionState state);

    sf::Time waitTime = sf::seconds(5);
    sf::Time accumulatedTime = sf::Time::Zero;
    FadeEffect fadeEffect;

    float m_WarningMessageTimer = 0.0f;
    const float WARNING_MESSAGE_DURATION = 4.0f; // 4 seconds
    const float NEWSPAPER_DURATION = 3.0f; // 3 seconds for newspaper display
    const float TIME_DISPLAY_DURATION = 3.0f; // 3 seconds for time and night text
    const float LOADING_SCREEN_DURATION = 2.0f; // 2 seconds wait for gameplay

private:
    // Static texture collections
    std::vector<std::shared_ptr<sf::Texture>> m_NoiseTextures;
    std::vector<std::shared_ptr<sf::Texture>> m_WhiteTextures;


    std::shared_ptr<sf::Font> font = nullptr;
};
