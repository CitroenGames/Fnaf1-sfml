#include "Graphics/SpriteLayout.h"

#include <algorithm>

namespace SpriteLayout {
    void SetOriginToCenter(sf::Sprite &sprite) {
        const sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    }

    void CenterAt(sf::Sprite &sprite, sf::Vector2f center) {
        SetOriginToCenter(sprite);
        sprite.setPosition(center);
    }

    void FitToSize(sf::Sprite &sprite, sf::Vector2f size, FitMode mode) {
        const sf::FloatRect bounds = sprite.getLocalBounds();
        if (bounds.width <= 0.0f || bounds.height <= 0.0f || size.x <= 0.0f || size.y <= 0.0f) {
            return;
        }

        const float scaleX = size.x / bounds.width;
        const float scaleY = size.y / bounds.height;
        const float scale = mode == FitMode::Cover
            ? std::max(scaleX, scaleY)
            : std::min(scaleX, scaleY);
        sprite.setScale(scale, scale);
    }

    void FitToSizeCentered(sf::Sprite &sprite, sf::Vector2f size, FitMode mode) {
        SetOriginToCenter(sprite);
        FitToSize(sprite, size, mode);
        sprite.setPosition(size.x * 0.5f, size.y * 0.5f);
    }
}
