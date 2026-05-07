#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace UI::Layout {
    inline sf::Vector2f CenteredPosition(sf::Vector2f areaSize, const sf::FloatRect &bounds) {
        return {
            (areaSize.x - bounds.width) * 0.5f - bounds.left,
            (areaSize.y - bounds.height) * 0.5f - bounds.top
        };
    }

    inline sf::Vector2f CenteredPosition(const sf::Vector2u &areaSize, const sf::FloatRect &bounds) {
        return CenteredPosition(
            sf::Vector2f(static_cast<float>(areaSize.x), static_cast<float>(areaSize.y)),
            bounds);
    }

    inline float CenteredX(float areaWidth, const sf::FloatRect &bounds) {
        return (areaWidth - bounds.width) * 0.5f - bounds.left;
    }

    inline float CenteredX(const sf::Vector2u &areaSize, const sf::FloatRect &bounds) {
        return CenteredX(static_cast<float>(areaSize.x), bounds);
    }
}
