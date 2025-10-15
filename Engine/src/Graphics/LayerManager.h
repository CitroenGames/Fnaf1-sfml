#pragma once

#include "Graphics/Layer.h"

class LayerManager {
public:
    static void AddDrawable(int layer, const sf::Drawable* drawable);
    static void ChangeLayer(const sf::Drawable* drawable, int newLayer);
    static void RemoveDrawable(const sf::Drawable* drawable);
    static void Clear();
    static void Draw(sf::RenderWindow& window);

private:
    static std::map<int, Layer> m_Layers;
    static std::map<const sf::Drawable*, int> m_DrawableToLayer;
};