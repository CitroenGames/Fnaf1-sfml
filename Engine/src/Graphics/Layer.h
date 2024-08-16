#include <SFML/Graphics.hpp>
#include <vector>

class Layer {
public:
    void addDrawable(const sf::Drawable& drawable) {
        m_Drawables.push_back(&drawable);
    }

    void removeDrawable(const sf::Drawable& drawable) {
        m_Drawables.erase(std::remove(m_Drawables.begin(), m_Drawables.end(), &drawable), m_Drawables.end());
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& drawable : m_Drawables) {
            window.draw(*drawable);
        }
    }

private:
    std::vector<const sf::Drawable*> m_Drawables;
};