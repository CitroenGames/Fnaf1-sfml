#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <array>
#include <functional>

using Timestamp = std::chrono::system_clock::time_point;

// Constants
constexpr int MAX_AI_LEVEL = 20;
constexpr double NORMAL_TIME_WINDOW = 3.02;  // Seconds between AI decisions
constexpr double ACCELERATED_TIME_WINDOW = 2.0;  // Faster time window for later nights
constexpr double POWER_DRAIN_BASE = 1.0;

namespace {
    constexpr float MAX_DELTA_TIME = 0.016667f; // Cap at 60 FPS equivalent (~16.7ms)
    constexpr float TIME_ACCELERATION_FACTOR = 0.85f;

    // Power consumption constants (now per second instead of per frame)
    constexpr float BASE_POWER_DRAIN_PER_SECOND = 0.1f;  // Reduced from 1.0f
    constexpr float CAMERA_POWER_MULTIPLIER = 0.15f;     // Additional 15% per second
    constexpr float DOOR_POWER_MULTIPLIER = 0.12f;       // Additional 12% per second
    constexpr float LIGHT_POWER_MULTIPLIER = 0.08f;      // Additional 8% per second
}

// Room definitions
enum class Room {
    SHOW_STAGE,
    DINING_AREA,
    PIRATE_COVE,
    WEST_HALL,
    EAST_HALL,
    WEST_CORNER,
    EAST_CORNER,
    SUPPLY_CLOSET,
    KITCHEN,
    RESTROOMS,
    OFFICE
};

struct AnimatronicPath {
    Room from;
    Room to;
    float probability;
};

struct AILevels
{
    int freddy, bonnie, chica, foxy;
};


class Animatronic {
public:
    std::string name;
    Room currentLocation;
    int aiLevel;
    float movementProgress;
    Timestamp lastMoved;
    bool isInOffice;
    std::vector<AnimatronicPath> possiblePaths;
    bool isActive;
    
    Animatronic(const std::string& name, int aiLevel);
    bool canMove(Room destination) const;
    void updateMovementProgress(float delta);
    void reset();
};

class FNAFGame {
public:
    FNAFGame();
    ~FNAFGame() = default;
    
    void InitializeGame(int night);
    void ShutdownGame() {}
    void InitializeCustomNight(AILevels _AILevels);
    void Update(float deltaTime);
    bool IsGameOver() const { return m_GameOver; }
    
private:
    // Game state
    bool m_GameOver;
    bool m_PowerOutage;
    float m_PowerOutageTimer;
    float m_FreddyMusicBoxTimer;
    std::map<std::string, std::unique_ptr<Animatronic>> m_Animatronics;
    std::mt19937 m_RNG;
    float m_TimeProgress = 0.0f;
    float m_CurrentHourDuration;

    // Movement tracking
    std::map<Room, std::vector<Room>> m_ValidMoves;
    std::array<bool, 2> m_Doors;  // [0] = left, [1] = right
    std::array<bool, 2> m_Lights; // [0] = left, [1] = right
    
    // AI and gameplay functions
    void UpdateAnimatronics(float deltaTime);
    void UpdatePower(float deltaTime);
    void CheckForJumpscare();
    void HandlePowerOutage(float deltaTime);
    bool IsDefendedAgainst(const Animatronic& animatronic) const;
    void TriggerJumpscare(const std::string& character);
    
    // Movement logic
    void AttemptMove(Animatronic& animatronic);
    bool ShouldAttemptMove(const Animatronic& animatronic);
    Room GetNextRoom(const Animatronic& animatronic) ;
    void MoveAnimatronic(Animatronic& animatronic, Room destination);
    
    // Character-specific behavior
    void UpdateFreddy(Animatronic& freddy, float deltaTime);
    void UpdateBonnie(Animatronic& bonnie, float deltaTime);
    void UpdateChica(Animatronic& chica, float deltaTime);
    void UpdateFoxy(Animatronic& foxy, float deltaTime);
    
    // Utility functions
    void InitializeMovementPaths();
    int GetAILevel(int night, const std::string& character) const;
    float CalculatePowerDrain() const;
    bool IsCameraViewingLocation(Room location) const;
    void PlaySound(const std::string& soundName) const;
};

