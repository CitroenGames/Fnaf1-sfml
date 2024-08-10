#pragma once

#include "ImageButton.h"
#include <functional>

class TopBottomButtons : public ImageButton {
public:
    void SetCallbacks(std::function<void()> onTopClick, std::function<void()> onBottomClick) {
		m_OnTopClick = onTopClick;
		m_OnBottomClick = onBottomClick;
	}

    void checkClick(sf::RenderWindow& window) {
        bool isCurrentlyPressed = (isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left));

        // Check if the button was just pressed this frame (not pressed last frame)
        if (isCurrentlyPressed && !m_IsPressed) {
            // Get mouse position in window coordinates
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Convert window coordinates to view coordinates
            sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);

            sf::FloatRect buttonBounds = m_ButtonSprite.getGlobalBounds();

            // Determine if click was in top or bottom half using updated bounds
            float relativeY = viewPos.y - buttonBounds.top;
            if (relativeY < buttonBounds.height / 2) {
                // Top half clicked
                if (m_OnTopClick) {
                    m_OnTopClick();
                }
            }
            else {
                // Bottom half clicked
                if (m_OnBottomClick) {
                    m_OnBottomClick();
                }
            }
        }

        // Update the pressed state for the next frame
        m_IsPressed = isCurrentlyPressed;
    }

private:
    // Callbacks for top and bottom clicks
    std::function<void()> m_OnTopClick;
    std::function<void()> m_OnBottomClick;

    // this should not be used
    bool isClicked(sf::RenderWindow& window) override { return m_IsPressed; };
};
