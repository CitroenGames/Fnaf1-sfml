#pragma once

#include "BaseButton.h"
#include "Core/Window.h"

// THIS CLASS IS NOT FINAL AT ALL IT LACKS MANY STUFF BUT WORKS WELL ENOUGH FOR THE GAME.
class HUDButton : public BaseButton {
public:
    HUDButton() {
        // Don't register with LayerManager in constructor
        // We'll handle drawing differently for HUD elements
    }

    ~HUDButton() {
        // Clean up LayerManager registration
        LayerManager::RemoveDrawable(this);
    }

    void SetPosition(float x, float y)
    {
        m_ScreenPosition.x = x;
        m_ScreenPosition.y = y;
        UpdatePosition();
    }

    void SetPosition(sf::Vector2f position)
    {
        m_ScreenPosition = position;
        UpdatePosition();
    }

    // Get the centered position (useful for debugging)
    sf::Vector2f GetScreenPosition() const
    {
        return m_ScreenPosition;
    }

    virtual void SetTexture(const std::string& textureFile)
    {
        m_ButtonTexture = Resources::GetTexture(textureFile);
        if (m_ButtonTexture) {
            sf::Sprite::setTexture(*m_ButtonTexture);
            CenterOrigin();
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
            CenterOrigin();
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
            CenterOrigin();
        }
        else {
            std::cerr << "Error: Provided texture is null." << std::endl;
        }
    }

    bool IsMouseOver(sf::RenderWindow& window) const override
    {
        // Save current view
        sf::View currentView = window.getView();
        
        // Temporarily switch to default view for correct mouse coordinates
        window.setView(window.getDefaultView());
        
        // Get mouse position in default view
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
        
        // Check if mouse is over button
        sf::FloatRect buttonBounds = getGlobalBounds();
        bool result = buttonBounds.contains(viewPos);
        
        // Restore original view
        window.setView(currentView);
        
        return result;
    }

    bool IsClicked(sf::RenderWindow& window) override
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

    void UpdatePosition()
    {
        // Set sprite position directly using screen position
        sf::Sprite::setPosition(m_ScreenPosition);
    }

    void Draw(sf::RenderWindow& window)
    {
        // Save current view
        sf::View currentView = window.getView();

        // Set to default view for drawing HUD
        window.setView(window.getDefaultView());

        // Draw button
        window.draw(*this);

        // Restore original view
        window.setView(currentView);
    }

    void Show()
    {
        if (!m_IsVisible) {
            m_IsVisible = true;
            LayerManager::AddDrawable(m_Layer, this);
        }
    }

    void Hide()
    {
        if (m_IsVisible) {
            m_IsVisible = false;
            LayerManager::RemoveDrawable(this);
        }
    }

protected:
    std::shared_ptr<sf::Texture> m_ButtonTexture;
    int m_Layer = 0;
    sf::Vector2f m_ScreenPosition; // Position relative to screen
    bool m_IsVisible = false;

    void CenterOrigin()
    {
        // Center origin of sprite for better positioning
        sf::FloatRect bounds = getLocalBounds();
        setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        UpdatePosition();
    }

private:
    using sf::Sprite::setPosition;
};