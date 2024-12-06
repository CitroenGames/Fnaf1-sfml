#pragma once
#define INITIAL_POWER_LEVEL 99

struct Player
{
    bool m_UsingCamera = false;
    bool m_UsingDoor = false;
    bool m_UsingLight = false;
    bool m_UsageLevel = 1;
    int m_Night = 5;
    float m_PowerLevel = INITIAL_POWER_LEVEL;
    float m_TimeSinceLastDrain = 0.0f;
};

inline Player player;