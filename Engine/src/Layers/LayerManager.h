#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include "Layer.h"

class LayerManager {
public:
    static void AddDrawable(int layer, const sf::Drawable& drawable);
    static void ChangeLayer(const sf::Drawable& drawable, int newLayer);
    static void RemoveDrawable(const sf::Drawable& drawable);
    static void Clear();
    static void Draw(sf::RenderWindow& window);

private:
    // Static members to hold layer and drawable information
    static std::map<int, Layer> Layers;
    static std::map<const sf::Drawable*, int> DrawableToLayer;
};