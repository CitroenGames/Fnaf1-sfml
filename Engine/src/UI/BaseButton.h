#pragma once    

class BaseButton : public sf::Sprite {
public:
    BaseButton(const sf::Texture& texture)
        : sf::Sprite(texture), m_IsPressed(false) {}

    virtual bool IsMouseOver(sf::RenderWindow& window) const { return false; }
    virtual bool IsClicked(sf::RenderWindow& window) { return false; }
protected:
    bool m_IsPressed;
};