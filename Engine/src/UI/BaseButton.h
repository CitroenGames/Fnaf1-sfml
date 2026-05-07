#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

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
    bool HandleLeftClick(bool isMouseOver);

    bool SetOwnedTexture(std::shared_ptr<sf::Texture> texture);

    std::shared_ptr<sf::Texture> m_ButtonTexture;
    bool m_IsPressed = false;
};
