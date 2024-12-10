#include "LayerManager.h"

std::map<int, Layer> LayerManager::m_Layers;
std::map<const sf::Drawable*, int> LayerManager::m_DrawableToLayer;

void LayerManager::AddDrawable(int layer, const sf::Drawable* drawable)
{
    m_Layers[layer].addDrawable(drawable);
    m_DrawableToLayer[drawable] = layer;
}

void LayerManager::ChangeLayer(const sf::Drawable* drawable, int newLayer)
{
    auto it = m_DrawableToLayer.find(drawable); 
    if (it != m_DrawableToLayer.end()) {
        int currentLayer = it->second;
        m_Layers[currentLayer].removeDrawable(drawable);
        AddDrawable(newLayer, drawable);
    }
}

void LayerManager::RemoveDrawable(const sf::Drawable* drawable)
{
    auto it = m_DrawableToLayer.find(drawable);
    if (it != m_DrawableToLayer.end()) {
        int layer = it->second;
        m_Layers[layer].removeDrawable(drawable); 
        m_DrawableToLayer.erase(it);
    }
}

void LayerManager::Clear()
{
    m_Layers.clear();
    m_DrawableToLayer.clear();
}

void LayerManager::Draw(sf::RenderWindow& window)
{
    for (const auto& layer : m_Layers) {
        layer.second.draw(window);
    }
}