#include "Graphics/FadeEffect.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

FadeEffect::FadeEffect()
    : m_Duration(sf::seconds(1.0f))
    , m_State(None)
    , m_ElapsedTime(sf::Time::Zero)
    , m_Alpha(0) {
}

FadeEffect::~FadeEffect() {
    m_Drawable = nullptr;
}

void FadeEffect::SetDrawable(sf::Drawable* drawable) {
    m_Drawable = drawable;
}

void FadeEffect::SetDuration(const sf::Time& duration) {
    m_Duration = duration;
}

void FadeEffect::StartFadeIn() {
    m_State = In;
    m_ElapsedTime = sf::Time::Zero;
    m_Alpha = 0;
}

void FadeEffect::StartFadeOut() {
    m_State = Out;
    m_ElapsedTime = sf::Time::Zero;
    m_Alpha = 255;
}

bool FadeEffect::Update(sf::Time deltaTime) {
    if (m_State == None || !m_Drawable) {
        return false;
    }

    m_ElapsedTime += deltaTime;
    const float progress = m_ElapsedTime.asSeconds() / m_Duration.asSeconds();

    if (progress >= 1.0f) {
        if (m_State == In) {
            m_Alpha = 255;
        } else if (m_State == Out) {
            m_Alpha = 0;
        }
        ApplyAlpha();
        m_State = None;
        return true;
    }

    if (m_State == In) {
        m_Alpha = static_cast<sf::Uint8>(255 * progress);
    } else if (m_State == Out) {
        m_Alpha = static_cast<sf::Uint8>(255 * (1.0f - progress));
    }

    ApplyAlpha();
    return false;
}

bool FadeEffect::IsFading() const {
    return m_State != None;
}

void FadeEffect::ApplyAlpha() {
    if (!m_Drawable) {
        return;
    }

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
