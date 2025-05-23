#pragma once

#define INITIAL_POWER_LEVEL 99

//AILevels CustomAILevels = { 0, 0, 0, 0 };

struct Player
{
    bool m_UsingCamera = false;
    bool m_UsingDoor = false;
    bool m_UsingLight = false;
    int m_UsageLevel = 1;
    int m_Night = 1;
    float m_PowerLevel = INITIAL_POWER_LEVEL;
    float m_TimeSinceLastDrain = 0.0f;
    uint8_t m_Time = 0;
};

inline Player player;