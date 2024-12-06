#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Composable.h"
#include "nlohmann/json.hpp"
#include "Player.h"
#include "FunctionSpeaker.h"

#define MAX_POWER_USAGE 5

class PowerIndicator : public Composable::Component {
public:
    // ECS Serialization Methods
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override {}
    std::string GetTypeName() const override { return "Power"; }

    // Component Lifecycle Methods
    void Init();
    void FixedUpdate() override {}
    void Update(double deltaTime) override;

    // Power Management Methods
    void SetUsageLevel(int level);
    float GetPowerLevel() const;
    void OnFoxyEvent(int eventCount);
    void ResetPower();

    MultiCastDelegate OnPowerDepletedDelegate;
private:
    // Rendering Components
    std::shared_ptr<sf::Font> m_Font;
    sf::Text m_PowerText;

    // Helper Methods for Power Drain Mechanics
    float CalculateDrainInterval(int usageLevel) const;
    float GetDrainIntervalForNight(int night) const;
    void UpdatePowerUsageText();
};