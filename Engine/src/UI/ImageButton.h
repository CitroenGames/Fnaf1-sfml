#pragma once

#include "BaseButton.h"

class ImageButton : public BaseButton {
public:
    void SetPosition(float x, float y) 
    {
        m_ButtonSprite.setPosition(x, y);
    }

    void SetPosition(sf::Vector2f position)
    {
        m_ButtonSprite.setPosition(position);
    }

    virtual void SetTexture(const std::string& textureFile) 
    {
        m_ButtonTexture = Resources::GetTexture(textureFile);
        if (m_ButtonTexture) {
            m_ButtonSprite.setTexture(*m_ButtonTexture);
            LayerManager::AddDrawable(m_Layer, *this);
        }
        else {
            std::cerr << "Error loading texture: " << textureFile << std::endl;
        }
    }

    void SetTexture(const sf::Texture& texture)
    {
		m_ButtonTexture = std::make_shared<sf::Texture>(texture);
		if (m_ButtonTexture) {
			m_ButtonSprite.setTexture(*m_ButtonTexture);
			LayerManager::AddDrawable(m_Layer, *this);
		}
		else {
			std::cerr << "Error: Provided texture is null." << std::endl;
		}
    }

    void SetTexture(std::shared_ptr<sf::Texture> texture) 
    {
        m_ButtonTexture = texture;
        if (m_ButtonTexture) {
            m_ButtonSprite.setTexture(*m_ButtonTexture);
            LayerManager::AddDrawable(m_Layer, *this);
        }
        else {
            std::cerr << "Error: Provided texture is null." << std::endl;
        }
    }

    bool IsMouseOver(sf::RenderWindow& window) const override
    {
        // Get mouse position in window coordinates
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Convert window coordinates to view coordinates
        sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);

        // Get button bounds
        sf::FloatRect buttonBounds = m_ButtonSprite.getGlobalBounds();

        // Check if the view position is within the button bounds
        return buttonBounds.contains(viewPos);
    }

    virtual bool IsClicked(sf::RenderWindow& window) override
    {
        bool isCurrentlyPressed = (IsMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left));

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
        m_Layer = layer;
        LayerManager::ChangeLayer(*this, layer);
    }

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_ButtonSprite, states);
    }
    sf::Sprite m_ButtonSprite;
    std::shared_ptr<sf::Texture> m_ButtonTexture;
    int m_Layer = 0;

private:
    void setPosition() = delete;
    void setPosition(sf::Vector2f) = delete;
    void setPosition(float, float) = delete;
};