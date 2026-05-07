#include "UI/BaseButton.h"

#include <utility>

#include <SFML/Window/Mouse.hpp>

#include "Core/Window.h"

bool BaseButton::IsMouseOver() const {
    const auto window = Window::GetWindow();
    return window && IsMouseOver(*window);
}

bool BaseButton::IsClicked() {
    const auto window = Window::GetWindow();
    return window && IsClicked(*window);
}

bool BaseButton::HandleLeftClick(bool isMouseOver) {
    const bool isCurrentlyPressed = isMouseOver && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (isCurrentlyPressed && !m_IsPressed) {
        m_IsPressed = true;
        return true;
    }

    if (!isCurrentlyPressed) {
        m_IsPressed = false;
    }

    return false;
}

bool BaseButton::SetOwnedTexture(std::shared_ptr<sf::Texture> texture) {
    m_ButtonTexture = std::move(texture);
    if (!m_ButtonTexture) {
        return false;
    }

    sf::Sprite::setTexture(*m_ButtonTexture);
    return true;
}
