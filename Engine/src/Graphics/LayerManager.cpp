#include "LayerManager.h"

std::map<int, Layer> LayerManager::m_Layers;
std::map<const sf::Drawable*, int> LayerManager::m_DrawableToLayer;

void LayerManager::AddDrawable(int layer, const sf::Drawable* drawable)
{
    if (!drawable) return;

    // Check if drawable is already in another layer
    auto it = m_DrawableToLayer.find(drawable);
    if (it != m_DrawableToLayer.end()) {
        // Remove from the old layer first
        int oldLayer = it->second;
        m_Layers[oldLayer].removeDrawable(drawable);
    }

    // Add to the new layer
    m_Layers[layer].addDrawable(drawable);
    m_DrawableToLayer[drawable] = layer;
}

void LayerManager::ChangeLayer(const sf::Drawable* drawable, int newLayer)
{
    if (!drawable) return;

    auto it = m_DrawableToLayer.find(drawable); 
    if (it != m_DrawableToLayer.end()) {
        int currentLayer = it->second;
        if (currentLayer != newLayer) {
            m_Layers[currentLayer].removeDrawable(drawable);
            m_Layers[newLayer].addDrawable(drawable);
            it->second = newLayer;
        }
    } else {
        // If drawable is not in any layer, add it directly
        AddDrawable(newLayer, drawable);
    }
}

void LayerManager::RemoveDrawable(const sf::Drawable* drawable)
{
    if (!drawable) return;

    auto it = m_DrawableToLayer.find(drawable);
    if (it != m_DrawableToLayer.end()) {
        int layer = it->second;
        m_Layers[layer].removeDrawable(drawable); 
        m_DrawableToLayer.erase(it);
    }
}

void LayerManager::Clear()
{
    // Clear all drawables from all layers
    for (auto& layer : m_Layers) {
        layer.second.clear();
    }
    
    // Clear our maps
    m_Layers.clear();
    m_DrawableToLayer.clear();
}

void LayerManager::Draw(sf::RenderWindow& window)
{
    // Draw layers in order
    for (const auto& layer : m_Layers) {
        layer.second.draw(window);
    }
}