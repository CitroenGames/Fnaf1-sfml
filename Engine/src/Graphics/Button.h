#pragma once
#include <SFML/Graphics.hpp>
#include "LayerManager.h"
#include "assets/Resources.h"
#include <iostream>
#include <memory>

class Button : public sf::Drawable {
public:
    Button(float x, float y, const std::string& textureFile, int layer)
        : m_Layer(layer)
    {
        SetTexture(textureFile);
        m_ButtonSprite.setPosition(x, y);
        LayerManager::AddDrawable(layer, *this);
    }

    ~Button() {}

    void setPosition(float x, float y) {
        m_ButtonSprite.setPosition(x, y);
    }

    void SetTexture(const std::string& textureFile) {
        m_ButtonTexture = Resources::GetTexture(textureFile);
        if (m_ButtonTexture) {
            m_ButtonSprite.setTexture(*m_ButtonTexture);
            LayerManager::AddDrawable(m_Layer, *this);
        }
        else {
            std::cerr << "Error loading texture: " << textureFile << std::endl;
        }
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
    std::shared_ptr<sf::Texture> m_ButtonTexture;
    int m_Layer;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_ButtonSprite, states);
    }
};