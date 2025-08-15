#pragma once

#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <numeric>

using Timestamp = std::chrono::system_clock::time_point;

// Random check ceiling (for 1-20 roll)
constexpr int MAX_RANDOM_ROLL = 20;

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

class CameraSystem;

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

    // FNAF 1 AI mechanics variables
    mutable float timeSinceLastMoveCheck;  // Time since last movement check
    float moveInterval;                    // Time between movement opportunities

    Animatronic(const std::string& name, int aiLevel);
    bool canMove(Room destination) const;
    void updateMovementProgress(float delta);
    void reset();
};

// Game events that can be subscribed to
enum class GameEvent {
    POWER_OUTAGE,
    JUMPSCARE,
    HOUR_CHANGE,
    // Add more events as needed
};

// Simple event system for game-wide notifications
class GameEvents {
public:
    // Register a callback for an event
    static void Subscribe(GameEvent event, std::function<void()> callback) {
        s_Subscribers[event].push_back(callback);
    }

    // Trigger an event
    static void TriggerEvent(GameEvent event) {
        auto it = s_Subscribers.find(event);
        if (it != s_Subscribers.end()) {
            for (auto& callback : it->second) {
                callback();
            }
        }
    }

private:
    static std::map<GameEvent, std::vector<std::function<void()>>> s_Subscribers;
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

    void SetCameraSystem(std::shared_ptr<CameraSystem> cameraSystem) {
        m_CameraSystem = cameraSystem;
    }

    Room GetAnimatronicLocation(const std::string& animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->currentLocation;
        }
        return Room::SHOW_STAGE; // Default location
    }

    float GetAnimatronicMovementProgress(const std::string& animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->movementProgress;
        }
        return 0.0f;
    }

    int GetAnimatronicAILevel(const std::string& animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->aiLevel;
        }
        return 0;
    }

    float GetAnimatronicTimeSinceLastMove(const std::string& animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->timeSinceLastMoveCheck;
        }
        return 0.0f;
    }

    float GetAnimatronicMoveInterval(const std::string& animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->moveInterval;
        }
        return 0.0f;
    }

    // Power system methods - simplified since state is now centralized
    bool IsPowerOutage() const { return m_PowerOutage; }
    float GetPowerOutageTimer() const { return m_PowerOutageTimer; }
    float GetFreddyMusicBoxTimer() const { return m_FreddyMusicBoxTimer; }
    bool IsDefendedAgainst(const Animatronic& animatronic) const;

    std::map<std::string, std::unique_ptr<Animatronic>> m_Animatronics;
private:
    // Game state
    bool m_GameOver;
    bool m_PowerOutage;
    float m_PowerOutageTimer;
    float m_FreddyMusicBoxTimer;
    std::mt19937 m_RNG;
    float m_TimeProgress = 0.0f;
    float m_CurrentHourDuration;
    int m_LastHourAIUpdated = 0;  // Tracks the last hour when AI was updated
    std::shared_ptr<CameraSystem> m_CameraSystem;  // Reference to the camera system

    // Movement tracking
    std::map<Room, std::vector<Room>> m_ValidMoves;

    // AI and gameplay functions
    void UpdateAnimatronics(float deltaTime);
    void UpdatePower(float deltaTime);
    void CheckForJumpscare();
    void HandlePowerOutage(float deltaTime);
    void TriggerJumpscare(const Animatronic& character);

    // Movement logic
    void AttemptMove(Animatronic& animatronic);
    bool ShouldAttemptMove(const Animatronic& animatronic);
    Room GetNextRoom(const Animatronic& animatronic);
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