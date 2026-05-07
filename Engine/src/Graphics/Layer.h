#pragma once

#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Layer {
public:
    Layer() = default;

    ~Layer() = default;

    void addDrawable(const sf::Drawable *drawable);

    void removeDrawable(const sf::Drawable *drawable);

    void draw(sf::RenderWindow &window) const;

    void clear();

private:
    std::vector<const sf::Drawable *> m_Drawables;
};
