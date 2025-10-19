#pragma once    

class BaseButton : public sf::Sprite {
public:
    BaseButton() : m_IsPressed(false) {}  // Initialize in constructor
    
    virtual bool IsMouseOver(sf::RenderWindow& window) const { return false; }

    virtual bool IsClicked(sf::RenderWindow& window) { return false; }

protected:
    bool m_IsPressed;
};