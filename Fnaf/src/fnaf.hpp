#pragma once

using Timestamp = std::chrono::system_clock::time_point;

// Random check ceiling (for 1-20 roll)
constexpr int MAX_RANDOM_ROLL = 20;

// Constants
constexpr int MAX_AI_LEVEL = 20;

namespace {
    constexpr float MAX_DELTA_TIME = 0.016667f; // Cap at 60 FPS equivalent (~16.7ms)
    constexpr float TIME_ACCELERATION_FACTOR = 0.85f;

    // Movement intervals (seconds) - from real FNAF 1
    constexpr float FREDDY_MOVE_INTERVAL = 3.02f;
    constexpr float BONNIE_MOVE_INTERVAL = 4.97f;
    constexpr float CHICA_MOVE_INTERVAL = 4.98f;
    constexpr float FOXY_MOVE_INTERVAL = 5.01f;

    // Power consumption constants (per 0.1 second tick, matching real game)
    constexpr float POWER_TICK_INTERVAL = 0.1f;
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

struct AILevels {
    int freddy, bonnie, chica, foxy;
};

class CameraSystem;

class Animatronic {
public:
    std::string name;
    Room currentLocation;
    int aiLevel;
    bool isActive;

    // FNAF 1 AI mechanics variables
    mutable float timeSinceLastMoveCheck = 0.0f;
    float moveInterval;

    // Foxy-specific: 4-stage progression (0=behind curtain, 1=peeking, 2=out, 3=gone/sprinting)
    int foxyStage = 0;
    int foxyDoorBangCount = 0;
    float foxyCooldownTimer = 0.0f; // Cooldown after camera is lowered before Foxy can move

    // Freddy-specific: post-move delay and office behavior
    float freddyMoveDelay = 0.0f; // Delay after a successful move before next move
    float freddyOfficeTimer = 0.0f; // Timer for 25% per second jumpscare chance in office

    Animatronic(const std::string &name, int aiLevel);
    void reset();
};

// Power outage phases (real FNAF 1 sequence)
enum class PowerOutagePhase {
    DARK_WAIT,    // Sit in darkness, 20% chance every 5s to advance (max 20s)
    FREDDY_FACE,  // Freddy appears in left door with music box, same timing
    LIGHTS_OFF,   // Brief blackout before jumpscare
    JUMPSCARE     // Freddy attacks
};

// Game events that can be subscribed to
enum class GameEvent {
    POWER_OUTAGE,
    JUMPSCARE,
    HOUR_CHANGE,
};

// Simple event system for game-wide notifications
class GameEvents {
public:
    static void Subscribe(GameEvent event, std::function<void()> callback) {
        s_Subscribers[event].push_back(callback);
    }

    static void TriggerEvent(GameEvent event) {
        auto it = s_Subscribers.find(event);
        if (it != s_Subscribers.end()) {
            for (auto &callback: it->second) {
                callback();
            }
        }
    }

private:
    static std::map<GameEvent, std::vector<std::function<void()> > > s_Subscribers;
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

    Room GetAnimatronicLocation(const std::string &animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->currentLocation;
        }
        return Room::SHOW_STAGE;
    }

    float GetAnimatronicMovementProgress(const std::string &animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            // For Foxy, return stage as percentage
            if (it->second->name == "Foxy") {
                return (it->second->foxyStage / 3.0f) * 100.0f;
            }
            return 0.0f;
        }
        return 0.0f;
    }

    int GetAnimatronicAILevel(const std::string &animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->aiLevel;
        }
        return 0;
    }

    float GetAnimatronicTimeSinceLastMove(const std::string &animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->timeSinceLastMoveCheck;
        }
        return 0.0f;
    }

    float GetAnimatronicMoveInterval(const std::string &animatronicName) const {
        auto it = m_Animatronics.find(animatronicName);
        if (it != m_Animatronics.end()) {
            return it->second->moveInterval;
        }
        return 0.0f;
    }

    // Power system methods
    bool IsPowerOutage() const { return m_PowerOutage; }
    PowerOutagePhase GetPowerOutagePhase() const { return m_PowerOutagePhase; }
    float GetPhaseTimer() const { return m_PhaseTimer; }

    bool IsDefendedAgainst(const Animatronic &animatronic) const;

    std::map<std::string, std::unique_ptr<Animatronic> > m_Animatronics;

private:
    // Game state
    bool m_GameOver;
    bool m_PowerOutage;
    PowerOutagePhase m_PowerOutagePhase;
    float m_PhaseTimer;        // Time spent in current outage phase
    float m_PhaseCheckTimer;   // Timer for 5-second probability checks
    std::mt19937 m_RNG;
    float m_TimeProgress = 0.0f;
    float m_CurrentHourDuration;
    int m_LastHourAIUpdated = 0;
    std::shared_ptr<CameraSystem> m_CameraSystem;
    float m_PowerTickAccumulator = 0.0f;

    // Foxy camera tracking
    bool m_WasCameraUp = false; // Track camera state changes for Foxy cooldown

    // AI and gameplay functions
    void UpdateAnimatronics(float deltaTime);
    void UpdatePower(float deltaTime);
    void CheckForJumpscare();
    void HandlePowerOutage(float deltaTime);
    void TriggerJumpscare(const Animatronic &character);

    // Character-specific AI (real FNAF 1 mechanics)
    void UpdateFreddy(Animatronic &freddy, float deltaTime);
    void UpdateBonnie(Animatronic &bonnie, float deltaTime);
    void UpdateChica(Animatronic &chica, float deltaTime);
    void UpdateFoxy(Animatronic &foxy, float deltaTime);

    // Utility functions
    int GetAILevel(int night, const std::string &character) const;
    float CalculatePowerDrain() const;
    bool IsCameraViewingLocation(Room location) const;
    void PlaySound(const std::string &soundName) const;

    // AI roll check
    bool RollAICheck(int aiLevel);

    // Room helpers
    Room GetRandomBonnieRoom();
    Room GetRandomAdjacentChicaRoom(Room current);
    Room GetNextFreddyRoom(Room current);
};
