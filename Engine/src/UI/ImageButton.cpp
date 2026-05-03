#include "UI/ImageButton.h"

#include <iostream>
#include <utility>

#include <SFML/Graphics/Rect.hpp>

#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"

ImageButton::ImageButton() {
    LayerManager::AddDrawable(m_Layer, this);
}

ImageButton::~ImageButton() {
    LayerManager::RemoveDrawable(this);
}

void ImageButton::SetPosition(float x, float y) {
    sf::Sprite::setPosition(x, y);
}

void ImageButton::SetPosition(sf::Vector2f position) {
    sf::Sprite::setPosition(position);
}

void ImageButton::SetVisible(bool visible) {
    if (visible) {
        LayerManager::AddDrawable(m_Layer, this);
    } else {
        LayerManager::RemoveDrawable(this);
    }
}

void ImageButton::SetTexture(const std::string& textureFile) {
    ApplyTexture(Resources::GetTexture(textureFile), "Error loading texture: " + textureFile);
}

void ImageButton::SetTexture(const sf::Texture& texture) {
    ApplyTexture(std::make_shared<sf::Texture>(texture), "Error: Provided texture is null.");
}

void ImageButton::SetTexture(std::shared_ptr<sf::Texture> texture) {
    ApplyTexture(std::move(texture), "Error: Provided texture is null.");
}

bool ImageButton::IsMouseOver(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
    sf::FloatRect buttonBounds = getGlobalBounds();
    return buttonBounds.contains(viewPos);
}

bool ImageButton::IsClicked(sf::RenderWindow& window) {
    return HandleLeftClick(IsMouseOver(window));
}

void ImageButton::SetLayer(int layer) {
    if (m_Layer != layer) {
        LayerManager::ChangeLayer(this, layer);
        m_Layer = layer;
    }
}

void ImageButton::ApplyTexture(std::shared_ptr<sf::Texture> texture, const std::string& errorMessage) {
    if (!SetOwnedTexture(std::move(texture))) {
        std::cerr << errorMessage << std::endl;
    }
}
