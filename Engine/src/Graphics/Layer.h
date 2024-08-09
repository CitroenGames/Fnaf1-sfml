#include <SFML/Graphics.hpp>
#include <vector>

class Layer {
public:
    void addDrawable(const sf::Drawable& drawable) {
        drawables.push_back(&drawable);
    }

    void removeDrawable(const sf::Drawable& drawable) {
        drawables.erase(std::remove(drawables.begin(), drawables.end(), &drawable), drawables.end());
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& drawable : drawables) {
            window.draw(*drawable);
        }
    }

private:
    std::vector<const sf::Drawable*> drawables;
};