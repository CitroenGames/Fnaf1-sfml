#pragma once

#include "BaseButton.h"
#include "Assets/Resources.h"
#include "utils/Helpers.h"

class ImageButton : public BaseButton, public sf::Sprite {
public:
    ImageButton() : sf::Sprite(GetDummyTexture()) {
        // Register with LayerManager in constructor
        LayerManager::AddDrawable(m_Layer, this);
    }

    ~ImageButton() {
        // Clean up LayerManager registration
        LayerManager::RemoveDrawable(this);
    }

    void SetPosition(float x, float y)
    {
        sf::Sprite::setPosition(sf::Vector2f(x, y));
    }

    void SetPosition(sf::Vector2f position)
    {
        sf::Sprite::setPosition(position);
    }

    void SetVisible(bool visible) {
        if (visible) {
            LayerManager::AddDrawable(m_Layer, this);
        }
        else {
            LayerManager::RemoveDrawable(this);
        }
    }

    virtual void SetTexture(const std::string& textureFile)
    {
        m_ButtonTexture = Resources::GetTexture(textureFile);
        if (m_ButtonTexture) {
            sf::Sprite::setTexture(*m_ButtonTexture);
        }
        else {
            std::cerr << "Error loading texture: " << textureFile << std::endl;
        }
    }

    void SetTexture(const sf::Texture& texture)
    {
        m_ButtonTexture = std::make_shared<sf::Texture>(texture);
        if (m_ButtonTexture) {
            sf::Sprite::setTexture(*m_ButtonTexture);
        }
        else {
            std::cerr << "Error: Provided texture is null." << std::endl;
        }
    }

    void SetTexture(std::shared_ptr<sf::Texture> texture)
    {
        m_ButtonTexture = texture;
        if (m_ButtonTexture) {
            sf::Sprite::setTexture(*m_ButtonTexture);
        }
        else {
            std::cerr << "Error: Provided texture is null." << std::endl;
        }
    }

    bool IsMouseOver(sf::RenderWindow& window) const override
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
        sf::FloatRect buttonBounds = getGlobalBounds();
        return buttonBounds.contains(viewPos);
    }

    virtual bool IsClicked(sf::RenderWindow& window) override
    {
        bool isCurrentlyPressed = (IsMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left));
        if (isCurrentlyPressed && !m_IsPressed)
        {
            m_IsPressed = true;
            return true;
        }
        if (!isCurrentlyPressed)
        {
            m_IsPressed = false;
        }
        return false;
    }

    void SetLayer(int layer)
    {
        if (m_Layer != layer) {
            LayerManager::ChangeLayer(this, layer);
            m_Layer = layer;
        }
    }

protected:
    std::shared_ptr<sf::Texture> m_ButtonTexture;
    int m_Layer = 0;

private:
    using sf::Sprite::setPosition;
};