#pragma once
#define INITIAL_POWER_LEVEL 99

struct Player
{
    bool m_UsingCamera = false;
    unsigned char m_UsingDoor = false;
    unsigned char m_UsingLight = false;
    unsigned char m_UsageLevel = 1;
    int m_Night = 5;
    float m_PowerLevel = INITIAL_POWER_LEVEL;
    float m_TimeSinceLastDrain = 0.0f;
};

inline Player player;