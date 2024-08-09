#pragma once

#include <SFML/Graphics.hpp>
#include "LayerManager.h"
#include <iostream>

class Button : public sf::Drawable {
public:
    Button(float x, float y, const std::string& textureFile, int layer)
        : m_Layer(layer)
    {
        // Load the button's texture
        if (!m_ButtonTexture.loadFromFile(textureFile)) {
            std::cerr << "Error loading texture\n";
        }

        // Set up the button's sprite
        m_ButtonSprite.setTexture(m_ButtonTexture);
        m_ButtonSprite.setPosition(x, y);

        // Add the button to the layer manager
        LayerManager::AddDrawable(layer, *this);
    }

    ~Button() {}

    void setPosition(float x, float y) {
        m_ButtonSprite.setPosition(x, y);
    }

    bool isMouseOver(sf::RenderWindow& window) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect buttonBounds = m_ButtonSprite.getGlobalBounds();
        return buttonBounds.contains(static_cast<sf::Vector2f>(mousePos));
    }

    bool isClicked(sf::RenderWindow& window) const {
        return (isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left));
    }

    void changeLayer(int newLayer) {
        LayerManager::ChangeLayer(*this, newLayer);
        m_Layer = newLayer;
    }

private:
    sf::Sprite m_ButtonSprite;
    sf::Texture m_ButtonTexture;
    int m_Layer;

    // Override the draw method from sf::Drawable
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_ButtonSprite, states);
    }
};
