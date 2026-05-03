#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "ImageButton.h"

class TopBottomButtons : public ImageButton {
public:
    enum class ButtonState {
        NoActive = 0,
        TopActive = 1,
        BothActive = 2,
        BottomActive = 3
    };

    TopBottomButtons();

    void SetCallbacks(
        std::function<void(bool active)> onTopClick,
        std::function<void(bool active)> onBottomClick
    );

    void SetTextures(const std::vector<std::shared_ptr<sf::Texture> > &textures);
    void updateButton();
    void updateButton(sf::RenderWindow &window);

    ButtonState GetCurrentState() const;

    void updateBottomState(bool active);
    void SetEnabled(bool IsEnabled);

private:
    std::function<void(bool active)> m_OnTopClick;
    std::function<void(bool active)> m_OnBottomClick;
    std::vector<std::shared_ptr<sf::Texture> > m_Textures;

    ButtonState m_CurrentState;
    bool m_WasMousePressed;
    bool m_Enabled = true;

    void updateTopState();
    void updateBottomState();
    void updateTexture();

    bool IsClicked(sf::RenderWindow &window) override;
};
