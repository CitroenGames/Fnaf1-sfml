#pragma once

#define INITIAL_POWER_LEVEL 99

struct Player
{
    bool m_UsingCamera = false;
    bool m_LeftDoorClosed = false;
    bool m_RightDoorClosed = false;
    bool m_LeftLightOn = false;
    bool m_RightLightOn = false;
    
    int m_UsageLevel = 1;
    int m_Night = 1;
    float m_PowerLevel = INITIAL_POWER_LEVEL;
    float m_TimeSinceLastDrain = 0.0f;
    uint8_t m_Time = 0;

    // Calculate usage level based on active systems
    int CalculateUsageLevel() const {
        int usage = 1; // Base usage
        
        if (m_UsingCamera) usage++;
        if (m_LeftDoorClosed) usage++;
        if (m_RightDoorClosed) usage++;
        if (m_LeftLightOn || m_RightLightOn) usage++;
        
        return std::min(5, usage);
    }

    // Update the usage level
    void UpdateUsageLevel() {
        m_UsageLevel = CalculateUsageLevel();
    }
};

inline Player player;