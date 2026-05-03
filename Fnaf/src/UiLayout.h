#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace UiLayout {
    inline sf::Vector2f CenteredPosition(const sf::Vector2u &windowSize, const sf::FloatRect &bounds) {
        return {
            (static_cast<float>(windowSize.x) - bounds.width) / 2.0f,
            (static_cast<float>(windowSize.y) - bounds.height) / 2.0f
        };
    }

    inline float CenteredX(const sf::Vector2u &windowSize, const sf::FloatRect &bounds) {
        return (static_cast<float>(windowSize.x) - bounds.width) / 2.0f;
    }
}
