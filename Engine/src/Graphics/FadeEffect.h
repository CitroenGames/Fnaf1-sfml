#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class FadeEffect {
public:
    enum FadeState { In, Out, None };

    FadeEffect() : m_State(None), m_ElapsedTime(sf::Time::Zero), m_Alpha(0), 
                   m_Duration(sf::seconds(1.0f)) {}

    ~FadeEffect() {
        // Ensure drawable is properly reset
        m_Drawable = nullptr;
    }

    void SetDrawable(sf::Drawable* drawable) {
        m_Drawable = drawable;
    }

    void SetDuration(const sf::Time& duration) {
        m_Duration = duration;
    }

    void StartFadeIn() {
        m_State = In;
        m_ElapsedTime = sf::Time::Zero;
        m_Alpha = 0;
    }

    void StartFadeOut() {
        m_State = Out;
        m_ElapsedTime = sf::Time::Zero;
        m_Alpha = 255;
    }

    bool Update(sf::Time deltaTime) {
        if (m_State == None || !m_Drawable) return false;

        m_ElapsedTime += deltaTime;
        float progress = m_ElapsedTime.asSeconds() / m_Duration.asSeconds();

        if (progress >= 1.0f) {
            if (m_State == In) {
                m_Alpha = 255;
            } else if (m_State == Out) {
                m_Alpha = 0;
            }
            ApplyAlpha();
            m_State = None; // Fade completed
            return true;
        }

        if (m_State == In) {
            m_Alpha = static_cast<sf::Uint8>(255 * progress);
        } else if (m_State == Out) {
            m_Alpha = static_cast<sf::Uint8>(255 * (1.0f - progress));
        }

        ApplyAlpha();
        return false; // Fade not yet completed
    }

    bool IsFading() const {
        return m_State != None;
    }

private:
    sf::Drawable* m_Drawable = nullptr;
    sf::Time m_Duration;
    FadeState m_State;
    sf::Time m_ElapsedTime;
    sf::Uint8 m_Alpha;

    void ApplyAlpha() {
        if (!m_Drawable) return;
        
        sf::Color color;

        if (auto* shape = dynamic_cast<sf::Shape*>(m_Drawable)) {
            color = shape->getFillColor();
            color.a = m_Alpha;
            shape->setFillColor(color);
        } else if (auto* sprite = dynamic_cast<sf::Sprite*>(m_Drawable)) {
            color = sprite->getColor();
            color.a = m_Alpha;
            sprite->setColor(color);
        } else if (auto* text = dynamic_cast<sf::Text*>(m_Drawable)) {
            color = text->getFillColor();
            color.a = m_Alpha;
            text->setFillColor(color);
        } else if (auto* rectangleShape = dynamic_cast<sf::RectangleShape*>(m_Drawable)) {
            color = rectangleShape->getFillColor();
            color.a = m_Alpha;
            rectangleShape->setFillColor(color);
        }
    }
};