#include "LayerManager.h"

std::map<int, Layer> LayerManager::Layers;
std::map<const sf::Drawable*, int> LayerManager::DrawableToLayer;

void LayerManager::AddDrawable(int layer, const sf::Drawable& drawable)
{
    Layers[layer].addDrawable(drawable);
    DrawableToLayer[&drawable] = layer;
}

void LayerManager::ChangeLayer(const sf::Drawable& drawable, int newLayer)
{
    auto it = DrawableToLayer.find(&drawable);
    if (it != DrawableToLayer.end()) {
        int currentLayer = it->second;
        Layers[currentLayer].removeDrawable(drawable); // Remove from the current layer
        AddDrawable(newLayer, drawable);               // Add to the new layer
    }
}

void LayerManager::RemoveDrawable(const sf::Drawable& drawable)
{
    auto it = DrawableToLayer.find(&drawable);
    if (it != DrawableToLayer.end()) {
        int layer = it->second;
        Layers[layer].removeDrawable(drawable);
        DrawableToLayer.erase(it);
    }
}

void LayerManager::Clear()
{
    Layers.clear();
    DrawableToLayer.clear();
}

void LayerManager::Draw(sf::RenderWindow& window)
{
    for (const auto& layer : Layers) {
        layer.second.draw(window);
    }
}