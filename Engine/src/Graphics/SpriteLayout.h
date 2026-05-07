#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace SpriteLayout {
    enum class FitMode {
        Contain,
        Cover
    };

    void SetOriginToCenter(sf::Sprite &sprite);
    void CenterAt(sf::Sprite &sprite, sf::Vector2f center);
    void FitToSize(sf::Sprite &sprite, sf::Vector2f size, FitMode mode);
    void FitToSizeCentered(sf::Sprite &sprite, sf::Vector2f size, FitMode mode);
}
