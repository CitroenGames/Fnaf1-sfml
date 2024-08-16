#pragma once
#include <SFML/Graphics.hpp>

#include "Graphics/LayerManager.h"
#include "assets/Resources.h"
#include <iostream>
#include <memory>

class BaseButton : public sf::Sprite {
public:
    virtual bool IsMouseOver(sf::RenderWindow& window) const { return false; }

    virtual bool IsClicked(sf::RenderWindow& window) { return false; }

protected:
    bool m_IsPressed;
};