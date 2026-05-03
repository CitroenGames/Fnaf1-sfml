#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>

class FadeEffect {
public:
    enum FadeState { In, Out, None };

    FadeEffect();
    ~FadeEffect();

    void SetDrawable(sf::Drawable* drawable);
    void SetDuration(const sf::Time& duration);

    void StartFadeIn();
    void StartFadeOut();

    bool Update(sf::Time deltaTime);
    bool IsFading() const;

private:
    sf::Drawable* m_Drawable = nullptr;
    sf::Time m_Duration;
    FadeState m_State;
    sf::Time m_ElapsedTime;
    sf::Uint8 m_Alpha;

    void ApplyAlpha();
};
