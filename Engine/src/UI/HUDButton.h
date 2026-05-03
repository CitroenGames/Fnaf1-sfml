#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include "BaseButton.h"

class HUDButton : public BaseButton {
public:
    HUDButton();
    ~HUDButton();

    using BaseButton::IsClicked;
    using BaseButton::IsMouseOver;

    void SetPosition(float x, float y);
    void SetPosition(sf::Vector2f position);

    sf::Vector2f GetScreenPosition() const;

    virtual void SetTexture(const std::string& textureFile);
    void SetTexture(const sf::Texture& texture);
    void SetTexture(std::shared_ptr<sf::Texture> texture);

    bool IsMouseOver(sf::RenderWindow& window) const override;
    bool IsClicked(sf::RenderWindow& window) override;

    void UpdatePosition();
    void Draw(sf::RenderWindow& window);
    void Show();
    void Hide();

protected:
    int m_Layer = 0;
    sf::Vector2f m_ScreenPosition; // Position relative to screen
    bool m_IsVisible = false;

    void CenterOrigin();
    sf::Vector2f AdjustForViewport(const sf::Vector2f& position) const;

private:
    void ApplyTexture(std::shared_ptr<sf::Texture> texture, const std::string& errorMessage);

    using sf::Sprite::setPosition;
};
