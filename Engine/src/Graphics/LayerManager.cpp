#include "Graphics/LayerManager.h"

std::map<int, Layer> LayerManager::m_Layers;
std::map<const sf::Drawable *, int> LayerManager::m_DrawableToLayer;

void LayerManager::AddDrawable(int layer, const sf::Drawable *drawable) {
    if (!drawable) return;

    auto it = m_DrawableToLayer.find(drawable);
    if (it != m_DrawableToLayer.end()) {
        int oldLayer = it->second;
        m_Layers[oldLayer].removeDrawable(drawable);
    }

    m_Layers[layer].addDrawable(drawable);
    m_DrawableToLayer[drawable] = layer;
}

void LayerManager::ChangeLayer(const sf::Drawable *drawable, int newLayer) {
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
        AddDrawable(newLayer, drawable);
    }
}

void LayerManager::RemoveDrawable(const sf::Drawable *drawable) {
    if (!drawable) return;

    auto it = m_DrawableToLayer.find(drawable);
    if (it != m_DrawableToLayer.end()) {
        int layer = it->second;
        m_Layers[layer].removeDrawable(drawable);
        m_DrawableToLayer.erase(it);
    }
}

void LayerManager::Clear() {
    for (auto &layer: m_Layers) {
        layer.second.clear();
    }

    m_Layers.clear();
    m_DrawableToLayer.clear();
}

void LayerManager::Draw(sf::RenderWindow &window) {
    for (const auto &layer: m_Layers) {
        layer.second.draw(window);
    }
}
