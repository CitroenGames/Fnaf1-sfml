#pragma once

#include <memory>
#include <utility>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Mouse.hpp>

class BaseButton : public sf::Sprite {
public:
    virtual ~BaseButton() = default;

    virtual bool IsMouseOver() const;

    virtual bool IsMouseOver(sf::RenderWindow& window) const {
        (void) window;
        return false;
    }

    virtual bool IsClicked();

    virtual bool IsClicked(sf::RenderWindow& window) {
        (void) window;
        return false;
    }

protected:
    bool HandleLeftClick(bool isMouseOver) {
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

    bool SetOwnedTexture(std::shared_ptr<sf::Texture> texture) {
        m_ButtonTexture = std::move(texture);
        if (!m_ButtonTexture) {
            return false;
        }

        sf::Sprite::setTexture(*m_ButtonTexture);
        return true;
    }

    std::shared_ptr<sf::Texture> m_ButtonTexture;
    bool m_IsPressed = false;
};
