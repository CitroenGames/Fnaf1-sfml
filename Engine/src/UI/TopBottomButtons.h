#pragma once
#include "ImageButton.h"
#include <functional>
#include <vector>

class TopBottomButtons : public ImageButton {
public:
    TopBottomButtons() : m_ActiveState(0) { LayerManager::AddDrawable(0, m_ButtonSprite); }

    void SetCallbacks(std::function<void(bool active)> onTopClick, std::function<void(bool active)> onBottomClick) {
        m_OnTopClick = onTopClick;
        m_OnBottomClick = onBottomClick;
    }

    void SetTextures(const std::vector<sf::Texture>& textures) 
    {
        if (textures.empty()) {
            std::cerr << "Error: Provided texture vector is empty." << std::endl;
            return;
        }

        m_Textures = textures;
        SetTexture(m_Textures[0]);
    }

    void checkClick(sf::RenderWindow& window) 
    {
        bool isCurrentlyPressed = (IsMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left));

        if (isCurrentlyPressed && !m_IsPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f viewPos = window.mapPixelToCoords(mousePos);
            sf::FloatRect buttonBounds = m_ButtonSprite.getGlobalBounds();

            float relativeY = viewPos.y - buttonBounds.top;
            if (relativeY < buttonBounds.height / 2) {
                // Top half clicked
                if (m_OnTopClick) {
                    m_OnTopClick(true);
                }
                updateTexture(true);
            }
            else {
                // Bottom half clicked
                if (m_OnBottomClick) {
                    m_OnBottomClick(true);
                }
                updateTexture(false);
            }
        }

        m_IsPressed = isCurrentlyPressed;
    }

private:
    std::function<void(bool active)> m_OnTopClick;
    std::function<void(bool active)> m_OnBottomClick;
    std::vector<sf::Texture> m_Textures;
    int m_ActiveState;

    void updateTexture(bool isTopClicked) {
        if (m_Textures.empty()) return;

        if (isTopClicked) {
            m_ActiveState = (m_ActiveState + 1) % (m_Textures.size() / 2);
        }
        else {
            m_ActiveState = ((m_ActiveState + 1) % (m_Textures.size() / 2)) + (m_Textures.size() / 2);
        }

        m_ButtonSprite.setTexture(m_Textures[m_ActiveState]);
    }

    bool IsClicked(sf::RenderWindow& window) override { return m_IsPressed; }
};