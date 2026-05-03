#include "UI/TopBottomButtons.h"

#include <iostream>
#include <utility>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Mouse.hpp>

TopBottomButtons::TopBottomButtons()
    : m_CurrentState(ButtonState::NoActive)
    , m_WasMousePressed(false) {
}

void TopBottomButtons::SetCallbacks(
    std::function<void(bool active)> onTopClick,
    std::function<void(bool active)> onBottomClick
) {
    m_OnTopClick = std::move(onTopClick);
    m_OnBottomClick = std::move(onBottomClick);
}

void TopBottomButtons::SetTextures(const std::vector<std::shared_ptr<sf::Texture> > &textures) {
    if (textures.size() != 4) {
        std::cerr << "Error: Expected exactly 4 textures (NoActive, TopActive, BothActive, BottomActive)."
                  << std::endl;
        return;
    }

    m_Textures = textures;
    SetTexture(m_Textures[static_cast<int>(ButtonState::NoActive)]);
}

void TopBottomButtons::updateButton(sf::RenderWindow &window) {
    if (!m_Enabled) {
        return;
    }

    if (IsMouseOver(window)) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_WasMousePressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
            sf::FloatRect buttonBounds = getGlobalBounds();

            const float relativeY = viewPos.y - buttonBounds.top;
            if (relativeY < buttonBounds.height / 2) {
                updateTopState();
                if (m_OnTopClick) {
                    m_OnTopClick(
                        m_CurrentState == ButtonState::TopActive || m_CurrentState == ButtonState::BothActive);
                }
            } else {
                updateBottomState();
                if (m_OnBottomClick) {
                    m_OnBottomClick(
                        m_CurrentState == ButtonState::BottomActive || m_CurrentState == ButtonState::BothActive);
                }
            }

            m_WasMousePressed = true;
        }

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            m_WasMousePressed = false;
        }
    }
}

TopBottomButtons::ButtonState TopBottomButtons::GetCurrentState() const {
    return m_CurrentState;
}

void TopBottomButtons::updateBottomState(bool active) {
    if (active) {
        if (m_CurrentState == ButtonState::NoActive) {
            m_CurrentState = ButtonState::BottomActive;
        } else if (m_CurrentState == ButtonState::TopActive) {
            m_CurrentState = ButtonState::BothActive;
        }
    } else {
        if (m_CurrentState == ButtonState::BottomActive) {
            m_CurrentState = ButtonState::NoActive;
        } else if (m_CurrentState == ButtonState::BothActive) {
            m_CurrentState = ButtonState::TopActive;
        }
    }
    updateTexture();
}

void TopBottomButtons::SetEnabled(bool IsEnabled) {
    m_CurrentState = ButtonState::NoActive;
    m_WasMousePressed = false;
    m_Enabled = IsEnabled;
    updateTexture();
}

void TopBottomButtons::updateTopState() {
    switch (m_CurrentState) {
        case ButtonState::NoActive:
            m_CurrentState = ButtonState::TopActive;
            break;
        case ButtonState::BottomActive:
            m_CurrentState = ButtonState::BothActive;
            break;
        case ButtonState::TopActive:
            m_CurrentState = ButtonState::NoActive;
            break;
        case ButtonState::BothActive:
            m_CurrentState = ButtonState::BottomActive;
            break;
    }
    updateTexture();
}

void TopBottomButtons::updateBottomState() {
    switch (m_CurrentState) {
        case ButtonState::NoActive:
            m_CurrentState = ButtonState::BottomActive;
            break;
        case ButtonState::TopActive:
            m_CurrentState = ButtonState::BothActive;
            break;
        case ButtonState::BottomActive:
            m_CurrentState = ButtonState::NoActive;
            break;
        case ButtonState::BothActive:
            m_CurrentState = ButtonState::TopActive;
            break;
    }
    updateTexture();
}

void TopBottomButtons::updateTexture() {
    if (m_Textures.empty()) {
        return;
    }

    SetTexture(m_Textures[static_cast<int>(m_CurrentState)]);
}

bool TopBottomButtons::IsClicked(sf::RenderWindow &window) {
    return false;
}
