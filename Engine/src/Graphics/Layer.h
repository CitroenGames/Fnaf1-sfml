#include <SFML/Graphics.hpp>
#include <vector>

class Layer {
public:
    void addDrawable(const sf::Drawable* drawable) {
        m_Drawables.push_back(drawable);
    }

    void removeDrawable(const sf::Drawable* drawable) {
        auto it = std::find(m_Drawables.begin(), m_Drawables.end(), drawable);
        if (it != m_Drawables.end()) {
            m_Drawables.erase(it);
        }
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto* drawable : m_Drawables) {
            window.draw(*drawable);
        }
    }

private:
    std::vector<const sf::Drawable*> m_Drawables;
};