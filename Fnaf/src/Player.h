#pragma once
#include <cstdint>

struct Player
{
    bool m_UsingCamera = false;
    bool m_UsingDoor = false;
    bool m_UsingLight = false;
    bool m_UsageLevel = 1;
    int m_Night = 1;
    float m_PowerLevel = 99.0f;
    float m_TimeSinceLastDrain = 0.0f;
    uint8_t m_Time = 0;

    void ResetForNight() {
        m_UsingCamera = false;
        m_UsingDoor = false;
        m_UsingLight = false;
        m_UsageLevel = 1;
        m_PowerLevel = 99.0f;
        m_TimeSinceLastDrain = 0.0f;
        m_Time = 0;
    }
};

inline Player player;
