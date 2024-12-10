#pragma once
#include "ImageButton.h"
#include <functional>
#include <vector>

class TopBottomButtons : public ImageButton {
public:
    enum class ButtonState {
        NoActive = 0,
        TopActive = 1,
        BothActive = 2,
        BottomActive = 3
    };

    TopBottomButtons() :
        m_CurrentState(ButtonState::NoActive),
        m_WasMousePressed(false) {}

    void SetCallbacks(
        std::function<void(bool active)> onTopClick,
        std::function<void(bool active)> onBottomClick
    ) {
        m_OnTopClick = onTopClick;
        m_OnBottomClick = onBottomClick;
    }

    void SetTextures(const std::vector<std::shared_ptr<sf::Texture>>& textures) {
        if (textures.size() != 4) {
            std::cerr << "Error: Expected exactly 4 textures (NoActive, TopActive, BothActive, BottomActive)." << std::endl;
            return;
        }

        m_Textures = textures;

        // Start with first (NoActive) texture
        SetTexture(m_Textures[static_cast<int>(ButtonState::NoActive)]);
    }

    void updateButton(sf::RenderWindow& window) {
        // Check if mouse is over button
        if (IsMouseOver(window)) {
            // Detect mouse press (only trigger once per click)
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_WasMousePressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
                sf::FloatRect buttonBounds = getGlobalBounds();  // Use inherited method

                float relativeY = viewPos.y - buttonBounds.top;

                if (relativeY < buttonBounds.height / 2) {
                    // Top half clicked
                    updateTopState();
                    if (m_OnTopClick) {
                        m_OnTopClick(m_CurrentState == ButtonState::TopActive || m_CurrentState == ButtonState::BothActive);
                    }
                }
                else {
                    // Bottom half clicked
                    updateBottomState();
                    if (m_OnBottomClick) {
                        m_OnBottomClick(m_CurrentState == ButtonState::BottomActive || m_CurrentState == ButtonState::BothActive);
                    }
                }

                // Mark that mouse was pressed to prevent repeated triggers
                m_WasMousePressed = true;
            }

            // Reset flag when mouse button is released
            if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                m_WasMousePressed = false;
            }
        }
    }

    ButtonState GetCurrentState() const { return m_CurrentState; }

private:
    std::function<void(bool active)> m_OnTopClick;
    std::function<void(bool active)> m_OnBottomClick;
    std::vector<std::shared_ptr<sf::Texture>> m_Textures;

    ButtonState m_CurrentState;
    bool m_WasMousePressed;    // Flag to prevent multiple triggers

    void updateTopState() {
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

    void updateBottomState() {
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

    void updateTexture() {
        if (m_Textures.empty()) return;

        // Update texture using inherited SetTexture method
        SetTexture(m_Textures[static_cast<int>(m_CurrentState)]);
    }

    bool IsClicked(sf::RenderWindow& window) override {
        return false; // Disable base class click detection
    }
};