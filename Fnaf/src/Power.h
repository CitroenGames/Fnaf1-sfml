#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Composable.h"
#include "nlohmann/json.hpp"

#define MAX_POWER_USAGE 5

class PowerIndicator : public Composable::Component {
public:
    //ECS STUFF
    nlohmann::json Serialize()const override { return nlohmann::json(); };
    void Deserialize(const nlohmann::json& data) override {};
    std::string GetTypeName() const override { return "Power"; }
    void FixedUpdate() override {}

    void Init();
    void Update(double deltaTime) override;

    void SetUsageLevel(int level) {
        usageLevel = std::max(1, std::min(level, MAX_POWER_USAGE));
    }

    float getPowerLevel() const {
        return powerLevel;
    }

    void onFoxyEvent(int eventCount) {
        // Example: Reduce power based on Foxy events
        if (eventCount == 1) {
            powerLevel -= 1;
        }
        else if (eventCount == 2) {
            powerLevel -= 6;
        }
        else if (eventCount >= 3) {
            powerLevel -= 11 + (eventCount - 3) * 5;
        }
        if (powerLevel < 0) powerLevel = 0; // Ensure power does not go below 0
    }

    const int GetCurrentPowerUsage() const {
		return usageLevel;
	}

    void SetNight(int night) {
        m_Night = night;
	}

    void resetPower() {
        powerLevel = 99;
        timeSinceLastDrain = 0;
    }

private:
    bool m_UsingCamera = false;
    unsigned char m_UsingDoor = false;
    unsigned char m_UsingLight = false;
    unsigned char usageLevel;


    int m_Night = 0;
    float powerLevel;
    float timeSinceLastDrain;

    std::shared_ptr<sf::Font> font;
    std::shared_ptr<sf::Sprite> usageBars;
    sf::Text powerText;

    float calculateDrainInterval(int usageLevel) {
        // Calculate drain interval based on usage level
        switch (usageLevel) {
            case 1: return 9.6f;
            case 2: return 4.8f;
            case 3: return 3.2f;  // Average of 2.8s, 2.9s, 3.9s
            case 4: return 2.4f;  // Average of 1.9s, 2.9s
            default: return 9.6f;
        }
    }

    float getDrainIntervalForNight(int night) {
        switch (night) {
        case 1: return 50.0f;
        case 2: return 43.0f;
        case 3: return 42.0f;
        case 4: return 40.0f;
        default: return 38.0f;
        }
    }
};
