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
    LayerManager::AddDrawable(UI_LAYER, &m_PowerText);
}

void PowerIndicator::Update(double deltaTime) {
    player.m_TimeSinceLastDrain += deltaTime;

    // Calculate power drain based on usage and night
    float baseInterval = GetDrainIntervalForNight(player.m_Night);
    float powerMultiplier = CalculatePowerMultiplier();
    float drainInterval = baseInterval / powerMultiplier;

    // Drain power at calculated rate
    if (player.m_TimeSinceLastDrain >= drainInterval && player.m_PowerLevel > 0) {
        DrainPower(1.0f);
        player.m_TimeSinceLastDrain = 0;
    }

    // Check for power depletion
    if (player.m_PowerLevel <= 0) {
        OnPowerDepletedDelegate.ExecuteAll();
    }

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

float PowerIndicator::CalculatePowerMultiplier() const {
    float multiplier = GetBaseUsageMultiplier(player.m_UsageLevel);

    // Add multipliers for active systems
    if (player.m_UsingDoor) multiplier += DOOR_POWER_MULTIPLIER;
    if (player.m_UsingLight) multiplier += LIGHT_POWER_MULTIPLIER;
    if (player.m_UsingCamera) multiplier += CAMERA_POWER_MULTIPLIER;

    return multiplier;
}

float PowerIndicator::GetBaseUsageMultiplier(int usageLevel) const {
    switch (usageLevel) {
    case 1: return 1.0f;
    case 2: return 1.5f;
    case 3: return 2.0f;
    case 4: return 2.5f;
    case 5: return 3.0f;
    default: return 1.0f;
    }
}

void PowerIndicator::DrainPower(float amount) {
    player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - amount);
}

float PowerIndicator::CalculateDrainInterval(int usageLevel) const {
    // Base intervals adjusted for better gameplay balance
    switch (usageLevel) {
    case 1: return 9.6f;   // Base drain: 1% every 9.6 seconds
    case 2: return 6.4f;   // Moderate drain
    case 3: return 4.8f;   // High drain
    case 4: return 3.2f;   // Very high drain
    case 5: return 2.4f;   // Maximum drain
    default: return 9.6f;
    }
}

float PowerIndicator::GetDrainIntervalForNight(int night) const {
    // Night-specific base intervals (higher = slower drain)
    switch (night) {
    case 1: return 48.0f;  // Easiest night
    case 2: return 42.0f;  // Getting harder
    case 3: return 36.0f;  // Moderate difficulty
    case 4: return 30.0f;  // Hard
    case 5: return 24.0f;  // Very hard
    default: return 20.0f; // Maximum difficulty
    }
}

void PowerIndicator::UpdatePowerUsageText() {
    // Update the power level text
    m_PowerText.setString(std::to_string(static_cast<int>(player.m_PowerLevel)) + "%");
}
