#pragma once
#include <SFML/Graphics.hpp>

#include "Graphics/LayerManager.h"
#include "assets/Resources.h"
#include <iostream>
#include <memory>

class BaseButton : public sf::Drawable {
public:
    virtual bool isMouseOver(sf::RenderWindow& window) const { return false; }

    virtual bool isClicked(sf::RenderWindow& window) { return false; }

protected:
    bool m_IsPressed;
};