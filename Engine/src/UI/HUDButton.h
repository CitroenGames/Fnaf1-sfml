#pragma once

#include "BaseButton.h"
#include "Core/Window.h"
#include "Assets/Resources.h"

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

        // Get mouse position in screen space
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Apply the viewport transformation to the mouse coordinates
        // This is necessary to handle letterboxing/pillarboxing
        sf::FloatRect viewport = currentView.getViewport();
        sf::Vector2u windowSize = window.getSize();

        // Calculate the position in the actual rendered area
        sf::Vector2f viewPos;

        // Handle letterboxing/pillarboxing by applying the inverse viewport transformation
        if (viewport.width < 1.0f) { // Pillarboxing (black bars on sides)
            // Check if mouse is in the viewable area
            if (mousePos.x < viewport.left * windowSize.x ||
                mousePos.x >(viewport.left + viewport.width) * windowSize.x) {
                // Mouse is in black bar, not over any HUD element
                window.setView(currentView);
                return false;
            }

            // Adjust x-coordinate to account for pillarboxing
            float adjustedX = (mousePos.x - (viewport.left * windowSize.x)) / (viewport.width * windowSize.x) * windowSize.x;
            viewPos = window.mapPixelToCoords(sf::Vector2i(static_cast<int>(adjustedX), mousePos.y));
        }
        else if (viewport.height < 1.0f) { // Letterboxing (black bars on top/bottom)
            // Check if mouse is in the viewable area
            if (mousePos.y < viewport.top * windowSize.y ||
                mousePos.y >(viewport.top + viewport.height) * windowSize.y) {
                // Mouse is in black bar, not over any HUD element
                window.setView(currentView);
                return false;
            }

            // Adjust y-coordinate to account for letterboxing
            float adjustedY = (mousePos.y - (viewport.top * windowSize.y)) / (viewport.height * windowSize.y) * windowSize.y;
            viewPos = window.mapPixelToCoords(sf::Vector2i(mousePos.x, static_cast<int>(adjustedY)));
        }
        else {
            // No letterboxing/pillarboxing, normal mapping
            viewPos = window.mapPixelToCoords(mousePos);
        }

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
        sf::Sprite::setPosition(AdjustForViewport(m_ScreenPosition));
    }

    void Draw(sf::RenderWindow& window)
    {
        // Save current view
        sf::View currentView = window.getView();

        // Set to default view for drawing HUD
        window.setView(window.getDefaultView());

        // Update position based on current viewport (for letterboxing/pillarboxing)
        sf::FloatRect viewport = currentView.getViewport();
        sf::Vector2f originalPos = sf::Sprite::getPosition();

        // Adjust position for viewport if needed
        sf::Vector2f adjustedPos = AdjustForViewport(m_ScreenPosition);
        sf::Sprite::setPosition(adjustedPos);

        // Draw button
        window.draw(*this);

        // Restore original position and view
        sf::Sprite::setPosition(originalPos);
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

    // Helper function to adjust positions for letterboxing/pillarboxing
    sf::Vector2f AdjustForViewport(const sf::Vector2f& position) const
    {
        // Get the current window and active viewport
		auto window = Window::GetWindow();
        if (!window) return position;

        sf::View currentView = window->getView();
        sf::FloatRect viewport = currentView.getViewport();
        sf::Vector2u windowSize = window->getSize();

        sf::Vector2f adjustedPos = position;

        // Adjust for pillarboxing (black bars on sides)
        if (viewport.width < 1.0f) {
            // Scale the x-coordinate to fit within the visible area
            adjustedPos.x = (viewport.left * windowSize.x) + (position.x * viewport.width);
        }

        // Adjust for letterboxing (black bars on top/bottom)
        if (viewport.height < 1.0f) {
            // Scale the y-coordinate to fit within the visible area
            adjustedPos.y = (viewport.top * windowSize.y) + (position.y * viewport.height);
        }

        return adjustedPos;
    }

private:
    using sf::Sprite::setPosition;
};