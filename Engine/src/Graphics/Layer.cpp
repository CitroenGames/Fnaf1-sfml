#include "Graphics/Layer.h"

#include <algorithm>

void Layer::addDrawable(const sf::Drawable *drawable) {
    if (drawable) {
        m_Drawables.push_back(drawable);
    }
}

void Layer::removeDrawable(const sf::Drawable *drawable) {
    if (!drawable) {
        return;
    }

    auto it = std::find(m_Drawables.begin(), m_Drawables.end(), drawable);
    if (it != m_Drawables.end()) {
        m_Drawables.erase(it);
    }
}

void Layer::draw(sf::RenderWindow &window) const {
    for (const auto *drawable: m_Drawables) {
        if (drawable) {
            window.draw(*drawable);
        }
    }
}

void Layer::clear() {
    m_Drawables.clear();
}
