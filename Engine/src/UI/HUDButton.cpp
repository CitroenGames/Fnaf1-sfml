#include "UI/HUDButton.h"

#include <iostream>
#include <utility>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Assets/Resources.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"

HUDButton::HUDButton() = default;

HUDButton::~HUDButton() {
    LayerManager::RemoveDrawable(this);
}

void HUDButton::SetPosition(float x, float y) {
    m_ScreenPosition.x = x;
    m_ScreenPosition.y = y;
    UpdatePosition();
}

void HUDButton::SetPosition(sf::Vector2f position) {
    m_ScreenPosition = position;
    UpdatePosition();
}

sf::Vector2f HUDButton::GetScreenPosition() const {
    return m_ScreenPosition;
}

void HUDButton::SetTexture(const std::string& textureFile) {
    ApplyTexture(Resources::GetTexture(textureFile), "Error loading texture: " + textureFile);
}

void HUDButton::SetTexture(const sf::Texture& texture) {
    ApplyTexture(std::make_shared<sf::Texture>(texture), "Error: Provided texture is null.");
}

void HUDButton::SetTexture(std::shared_ptr<sf::Texture> texture) {
    ApplyTexture(std::move(texture), "Error: Provided texture is null.");
}

bool HUDButton::IsMouseOver(sf::RenderWindow& window) const {
    sf::View currentView = window.getView();
    window.setView(window.getDefaultView());

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect viewport = currentView.getViewport();
    sf::Vector2u windowSize = window.getSize();

    sf::Vector2f viewPos;
    if (viewport.width < 1.0f) {
        if (mousePos.x < viewport.left * windowSize.x ||
            mousePos.x > (viewport.left + viewport.width) * windowSize.x) {
            window.setView(currentView);
            return false;
        }

        float adjustedX = (mousePos.x - (viewport.left * windowSize.x)) /
                          (viewport.width * windowSize.x) * windowSize.x;
        viewPos = window.mapPixelToCoords(sf::Vector2i(static_cast<int>(adjustedX), mousePos.y));
    } else if (viewport.height < 1.0f) {
        if (mousePos.y < viewport.top * windowSize.y ||
            mousePos.y > (viewport.top + viewport.height) * windowSize.y) {
            window.setView(currentView);
            return false;
        }

        float adjustedY = (mousePos.y - (viewport.top * windowSize.y)) /
                          (viewport.height * windowSize.y) * windowSize.y;
        viewPos = window.mapPixelToCoords(sf::Vector2i(mousePos.x, static_cast<int>(adjustedY)));
    } else {
        viewPos = window.mapPixelToCoords(mousePos);
    }

    const sf::FloatRect buttonBounds = getGlobalBounds();
    const bool result = buttonBounds.contains(viewPos);

    window.setView(currentView);
    return result;
}

bool HUDButton::IsClicked(sf::RenderWindow& window) {
    return HandleLeftClick(IsMouseOver(window));
}

void HUDButton::UpdatePosition() {
    sf::Sprite::setPosition(AdjustForViewport(m_ScreenPosition));
}

void HUDButton::Draw(sf::RenderWindow& window) {
    sf::View currentView = window.getView();
    window.setView(window.getDefaultView());

    sf::Vector2f originalPos = sf::Sprite::getPosition();
    sf::Sprite::setPosition(AdjustForViewport(m_ScreenPosition));

    window.draw(*this);

    sf::Sprite::setPosition(originalPos);
    window.setView(currentView);
}

void HUDButton::Show() {
    if (!m_IsVisible) {
        m_IsVisible = true;
        LayerManager::AddDrawable(m_Layer, this);
    }
}

void HUDButton::Hide() {
    if (m_IsVisible) {
        m_IsVisible = false;
        LayerManager::RemoveDrawable(this);
    }
}

void HUDButton::CenterOrigin() {
    const sf::FloatRect bounds = getLocalBounds();
    setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    UpdatePosition();
}

sf::Vector2f HUDButton::AdjustForViewport(const sf::Vector2f& position) const {
    auto window = Window::GetWindow();
    if (!window) {
        return position;
    }

    sf::View currentView = window->getView();
    sf::FloatRect viewport = currentView.getViewport();
    sf::Vector2u windowSize = window->getSize();

    sf::Vector2f adjustedPos = position;
    if (viewport.width < 1.0f) {
        adjustedPos.x = (viewport.left * windowSize.x) + (position.x * viewport.width);
    }

    if (viewport.height < 1.0f) {
        adjustedPos.y = (viewport.top * windowSize.y) + (position.y * viewport.height);
    }

    return adjustedPos;
}

void HUDButton::ApplyTexture(std::shared_ptr<sf::Texture> texture, const std::string& errorMessage) {
    if (SetOwnedTexture(std::move(texture))) {
        CenterOrigin();
    } else {
        std::cerr << errorMessage << std::endl;
    }
}
