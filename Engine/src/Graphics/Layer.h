#pragma once

class Layer {
public:
    Layer() = default;

    ~Layer() = default;

    void addDrawable(const sf::Drawable *drawable) {
        if (drawable) {
            m_Drawables.push_back(drawable);
        }
    }

    void removeDrawable(const sf::Drawable *drawable) {
        if (drawable) {
            auto it = std::find(m_Drawables.begin(), m_Drawables.end(), drawable);
            if (it != m_Drawables.end()) {
                m_Drawables.erase(it);
            }
        }
    }

    void draw(sf::RenderWindow &window) const {
        for (const auto *drawable: m_Drawables) {
            if (drawable) {
                window.draw(*drawable);
            }
        }
    }

    void clear() {
        m_Drawables.clear();
    }

private:
    std::vector<const sf::Drawable *> m_Drawables;
};
