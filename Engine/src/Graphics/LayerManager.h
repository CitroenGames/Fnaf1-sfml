#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class LayerManager {
public:
    static void AddDrawable(int layer, const sf::Drawable *drawable);

    static void ChangeLayer(const sf::Drawable *drawable, int newLayer);

    static void RemoveDrawable(const sf::Drawable *drawable);

    static void Clear();

    static void Draw(sf::RenderWindow &window);
};
