#include "Power.h"
#include "Core/Window.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "LayerDefines.h"
#include "Player.h"
#include <imgui.h>
#include <algorithm>

void PowerIndicator::Init() {
    // Initialize screen dimensions
    int screenWidth = Window::GetWindow()->getSize().x;
    int screenHeight = Window::GetWindow()->getSize().y;

    // Load and set up font
    m_Font = Resources::GetFont("Font/five-nights-at-freddys.ttf");

    // Configure power level text
    m_PowerText.setFont(*m_Font);
    m_PowerText.setCharacterSize(24);
    m_PowerText.setFillColor(sf::Color::White);
    m_PowerText.setPosition(screenWidth * 0.05f, screenHeight * 0.85f);

    // Add text to UI layer
    LayerManager::AddDrawable(UI_LAYER, m_PowerText);
}

void PowerIndicator::Update(double deltaTime) {
    player.m_TimeSinceLastDrain += deltaTime;

    // Existing power drain logic
    float drainInterval = CalculateDrainInterval(player.m_UsageLevel);
    if (player.m_TimeSinceLastDrain >= drainInterval && player.m_PowerLevel > 0) {
        player.m_PowerLevel--;
        player.m_TimeSinceLastDrain = 0;
    }

    if (player.m_PowerLevel <= 0) {
        OnPowerDepletedDelegate.ExecuteAll();
    }

    // Update power text
    UpdatePowerUsageText();
}

void PowerIndicator::SetUsageLevel(int level) {
    player.m_UsageLevel = std::max(1, std::min(level, MAX_POWER_USAGE));
}

float PowerIndicator::GetPowerLevel() const {
    return player.m_PowerLevel;
}

void PowerIndicator::OnFoxyEvent(int eventCount) {
    // Power drain based on Foxy interaction events
    switch (eventCount) {
    case 1:
        player.m_PowerLevel -= 1;
        break;
    case 2:
        player.m_PowerLevel -= 6;
        break;
    default:
        if (eventCount >= 3) {
            player.m_PowerLevel -= 11 + (eventCount - 3) * 5;
        }
        break;
    }

    // Ensure power does not go below 0
    player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel);
}

void PowerIndicator::ResetPower() {
    player.m_PowerLevel = INITIAL_POWER_LEVEL;
    player.m_TimeSinceLastDrain = 0.0f;
}

float PowerIndicator::CalculateDrainInterval(int usageLevel) const {
    // Power drain intervals based on usage level
    switch (usageLevel) {
    case 1: return 9.6f;  // Base drain: 1% every 9.6 seconds
    case 2: return 4.8f;  // 1% every 4.8 seconds
    case 3: return 3.2f;  // Average of 2.8s, 2.9s, 3.9s
    case 4: return 2.4f;  // Average of 1.9s, 2.9s
    default: return 9.6f;
    }
}

float PowerIndicator::GetDrainIntervalForNight(int night) const {
    // Night-specific adjustments to make the game progressively harder
    switch (night) {
    case 1: return 50.0f;
    case 2: return 43.0f;
    case 3: return 42.0f;
    case 4: return 40.0f;
    default: return 38.0f;
    }
}

void PowerIndicator::UpdatePowerUsageText() {
    // Update the power level text
    m_PowerText.setString(std::to_string(static_cast<int>(player.m_PowerLevel)) + "%");
}