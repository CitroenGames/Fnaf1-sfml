#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

class BaseButton : public sf::Sprite {
public:
    virtual bool IsMouseOver(sf::RenderWindow& window) const { return false; }

    virtual bool IsClicked(sf::RenderWindow& window) { return false; }

protected:
    bool m_IsPressed = false;
};
