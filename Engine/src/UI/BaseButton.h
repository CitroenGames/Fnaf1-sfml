#pragma once    

class BaseButton {
public:
    BaseButton()
        : m_IsPressed(false) {}

    virtual bool IsMouseOver(sf::RenderWindow& window) const { return false; }
    virtual bool IsClicked(sf::RenderWindow& window) { return false; }
protected:
    bool m_IsPressed;
};