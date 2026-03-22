#include "fnaf.hpp"

#include "Assets/Resources.h"
#include "GameState.h"
#include "CameraSystem.h"

namespace {
    constexpr float FREDDY_LAUGH_CHANCE = 0.1f;
constexpr float BASE_SECONDS_PER_HOUR = 89.0f;

    // Hour-based AI increments - from real FNAF 1
    // At 2AM: Bonnie +1
    // At 3AM: Bonnie +1, Chica +1, Foxy +1
    // At 4AM: Bonnie +1, Chica +1, Foxy +1
    // Freddy NEVER gets increments
    const std::map<int, std::map<std::string, int>> HOUR_AI_INCREMENTS = {
        {2, {{"Bonnie", 1}}},
        {3, {{"Bonnie", 1}, {"Chica", 1}, {"Foxy", 1}}},
        {4, {{"Bonnie", 1}, {"Chica", 1}, {"Foxy", 1}}}
    };

    // Bonnie's possible rooms (left side of building)
    const std::vector<Room> BONNIE_ROOMS = {
        Room::DINING_AREA, Room::SUPPLY_CLOSET, Room::WEST_HALL, Room::WEST_CORNER
    };

    // Chica's right-side room adjacency map
    const std::map<Room, std::vector<Room>> CHICA_ADJACENCY = {
        {Room::SHOW_STAGE,   {Room::DINING_AREA}},
        {Room::DINING_AREA,  {Room::KITCHEN, Room::RESTROOMS, Room::EAST_HALL}},
        {Room::KITCHEN,      {Room::DINING_AREA, Room::EAST_HALL}},
        {Room::RESTROOMS,    {Room::DINING_AREA, Room::EAST_HALL}},
        {Room::EAST_HALL,    {Room::DINING_AREA, Room::EAST_CORNER}},
        {Room::EAST_CORNER,  {}} // From corner, next move is office attempt
    };

    // Freddy's deterministic path
    const std::vector<Room> FREDDY_PATH = {
        Room::SHOW_STAGE, Room::DINING_AREA, Room::KITCHEN,
        Room::RESTROOMS, Room::EAST_HALL, Room::EAST_CORNER
    };

    // Night-based power drain divisors (from real game)
    const std::array<float, 7> NIGHT_POWER_DIVISORS = {
        9.6f, // Night 1
        6.0f, // Night 2
        5.0f, // Night 3
        4.0f, // Night 4
        3.0f, // Night 5
        3.0f, // Night 6
        3.0f  // Night 7 (Custom)
    };
}

// Initialize static member for GameEvents
std::map<GameEvent, std::vector<std::function<void()>>> GameEvents::s_Subscribers;

Animatronic::Animatronic(const std::string &name, int aiLevel)
    : name(name)
    , currentLocation(Room::SHOW_STAGE)
    , aiLevel(aiLevel)
    , isActive(true)
    , timeSinceLastMoveCheck(0.0f)
{
    // Set character-specific move intervals (from real FNAF 1)
    if (name == "Freddy") {
        moveInterval = FREDDY_MOVE_INTERVAL;
    } else if (name == "Bonnie") {
        moveInterval = BONNIE_MOVE_INTERVAL;
    } else if (name == "Chica") {
        moveInterval = CHICA_MOVE_INTERVAL;
    } else if (name == "Foxy") {
        moveInterval = FOXY_MOVE_INTERVAL;
        currentLocation = Room::PIRATE_COVE;
    }
}

void Animatronic::reset() {
    if (name == "Foxy") {
        currentLocation = Room::PIRATE_COVE;
        // Reset to stage 0 or 1 (random in real game, we'll use 0)
        foxyStage = 0;
    } else {
        currentLocation = Room::SHOW_STAGE;
    }
    timeSinceLastMoveCheck = 0.0f;
    freddyMoveDelay = 0.0f;
    freddyOfficeTimer = 0.0f;
}

FNAFGame::FNAFGame()
    : m_GameOver(false)
    , m_PowerOutage(false)
    , m_PowerOutagePhase(PowerOutagePhase::DARK_WAIT)
    , m_PhaseTimer(0.0f)
    , m_PhaseCheckTimer(0.0f)
    , m_RNG(std::random_device{}())
    , m_LastHourAIUpdated(0)
    , m_CameraSystem(nullptr)
    , m_WasCameraUp(false)
{
}

void FNAFGame::InitializeGame(int night) {
    player.m_Night = night;
    player.m_PowerLevel = INITIAL_POWER_LEVEL;
    player.m_Time = 0;
    m_TimeProgress = 0.0f;
    m_CurrentHourDuration = BASE_SECONDS_PER_HOUR;
    m_GameOver = false;
    m_PowerOutage = false;
    m_LastHourAIUpdated = 0;
    m_PowerTickAccumulator = 0.0f;

    // Reset player state
    player.m_UsingCamera = false;
    player.m_LeftDoorClosed = false;
    player.m_RightDoorClosed = false;
    player.m_LeftLightOn = false;
    player.m_RightLightOn = false;
    player.UpdateUsageLevel();

    // Initialize animatronics with real FNAF 1 AI levels
    m_Animatronics["Freddy"] = std::make_unique<Animatronic>("Freddy", GetAILevel(night, "Freddy"));
    m_Animatronics["Bonnie"] = std::make_unique<Animatronic>("Bonnie", GetAILevel(night, "Bonnie"));
    m_Animatronics["Chica"] = std::make_unique<Animatronic>("Chica", GetAILevel(night, "Chica"));
    m_Animatronics["Foxy"] = std::make_unique<Animatronic>("Foxy", GetAILevel(night, "Foxy"));
}

void FNAFGame::Update(float deltaTime) {
    if (m_GameOver) return;

    float clampedDelta = std::min(deltaTime, MAX_DELTA_TIME);

    UpdatePower(clampedDelta);

    // Calculate hour progress with time acceleration
    float acceleratedDelta = clampedDelta * std::pow(TIME_ACCELERATION_FACTOR, player.m_Time);
    m_TimeProgress += acceleratedDelta / m_CurrentHourDuration;

    if (m_TimeProgress >= 1.0f) {
        m_TimeProgress -= 1.0f;
        player.m_Time++;

        if (player.m_Time >= 6) {
            m_GameOver = true;
            return;
        }
    }

    UpdateAnimatronics(clampedDelta);
    CheckForJumpscare();
}

// ============================================================================
// POWER SYSTEM - Real FNAF 1 mechanics
// ============================================================================

void FNAFGame::UpdatePower(float deltaTime) {
    if (!m_PowerOutage) {
        float drainRate = CalculatePowerDrain();
        player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - (drainRate * deltaTime));

        if (player.m_PowerLevel <= 0) {
            m_PowerOutage = true;
            m_PowerOutagePhase = PowerOutagePhase::DARK_WAIT;
            m_PhaseTimer = 0.0f;
            m_PhaseCheckTimer = 0.0f;

            player.m_UsingCamera = false;
            player.m_LeftDoorClosed = false;
            player.m_RightDoorClosed = false;
            player.m_LeftLightOn = false;
            player.m_RightLightOn = false;
            player.UpdateUsageLevel();

            GameEvents::TriggerEvent(GameEvent::POWER_OUTAGE);
        }
        return;
    }

    HandlePowerOutage(deltaTime);
}

float FNAFGame::CalculatePowerDrain() const {
    // Real FNAF 1 power drain formula (decompiled):
    // Internal power = 999 (displayed as 99.9%). Every 0.1s tick:
    //   - Subtract usageLevel from internal counter
    //   - Every nightDivisor seconds: subtract 1 extra
    // Per second in internal units: 10*usageLevel + 1/nightDivisor
    // Converted to displayed %/s: (usageLevel + 0.1/nightDivisor) / 10
    float usageLevel = static_cast<float>(player.CalculateUsageLevel());

    int nightIdx = std::clamp(player.m_Night - 1, 0, 6);
    float divisor = NIGHT_POWER_DIVISORS[nightIdx];

    return (usageLevel + 0.1f / divisor) / 10.0f;
}

// ============================================================================
// ANIMATRONIC AI - Real FNAF 1 mechanics
// ============================================================================

void FNAFGame::UpdateAnimatronics(float deltaTime) {
    // Track camera state changes for Foxy
    bool cameraUp = player.m_UsingCamera;

    for (auto &[name, animatronic] : m_Animatronics) {
        if (!animatronic->isActive || animatronic->aiLevel <= 0) continue;

        // Check for hour-based AI increments
        if (player.m_Time > m_LastHourAIUpdated) {
            auto hourIt = HOUR_AI_INCREMENTS.find(player.m_Time);
            if (hourIt != HOUR_AI_INCREMENTS.end()) {
                auto nameIt = hourIt->second.find(name);
                if (nameIt != hourIt->second.end()) {
                    animatronic->aiLevel = std::min(animatronic->aiLevel + nameIt->second, MAX_AI_LEVEL);
                }
            }
        }

        // Character-specific AI updates
        if (name == "Freddy") {
            UpdateFreddy(*animatronic, deltaTime);
        } else if (name == "Bonnie") {
            UpdateBonnie(*animatronic, deltaTime);
        } else if (name == "Chica") {
            UpdateChica(*animatronic, deltaTime);
        } else if (name == "Foxy") {
            UpdateFoxy(*animatronic, deltaTime);
        }
    }

    // Update hour tracking after processing all animatronics
    if (player.m_Time > m_LastHourAIUpdated) {
        m_LastHourAIUpdated = player.m_Time;
    }

    m_WasCameraUp = cameraUp;
}

bool FNAFGame::RollAICheck(int aiLevel) {
    if (aiLevel <= 0) return false;
    if (aiLevel >= MAX_RANDOM_ROLL) return true;
    std::uniform_int_distribution<int> dist(1, MAX_RANDOM_ROLL);
    return dist(m_RNG) <= aiLevel;
}

// ============================================================================
// FREDDY AI - Deterministic path, camera stalling, special office behavior
// ============================================================================

void FNAFGame::UpdateFreddy(Animatronic &freddy, float deltaTime) {
    // Freddy cannot leave the stage while Bonnie or Chica are still there
    if (freddy.currentLocation == Room::SHOW_STAGE) {
        auto &bonnie = m_Animatronics["Bonnie"];
        auto &chica = m_Animatronics["Chica"];
        if (bonnie->currentLocation == Room::SHOW_STAGE ||
            chica->currentLocation == Room::SHOW_STAGE) {
            return; // Can't move yet
        }
    }

    // If Freddy is in the office, handle jumpscare timing
    if (freddy.currentLocation == Room::OFFICE) {
        // 25% chance per second to jumpscare, only when cameras are DOWN
        if (!player.m_UsingCamera) {
            freddy.freddyOfficeTimer += deltaTime;
            if (freddy.freddyOfficeTimer >= 1.0f) {
                freddy.freddyOfficeTimer -= 1.0f;
                if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < 0.25f) {
                    TriggerJumpscare(freddy);
                }
            }
        }
        return;
    }

    // Post-move delay (Freddy waits after a successful move)
    if (freddy.freddyMoveDelay > 0.0f) {
        freddy.freddyMoveDelay -= deltaTime;
        return;
    }

    // Camera stalling: if ANY camera is up, Freddy auto-fails movement
    // Exception: at EAST_CORNER, cameras being up is actually required for him to enter office
    if (freddy.currentLocation != Room::EAST_CORNER && player.m_UsingCamera) {
        freddy.timeSinceLastMoveCheck = 0.0f;
        return;
    }

    // Movement opportunity timer
    freddy.timeSinceLastMoveCheck += deltaTime;
    if (freddy.timeSinceLastMoveCheck < freddy.moveInterval) {
        return;
    }
    freddy.timeSinceLastMoveCheck = 0.0f;

    // At EAST_CORNER (4B): special entry rules
    if (freddy.currentLocation == Room::EAST_CORNER) {
        // Freddy can only enter office when:
        // - Cameras are UP
        // - Right door is OPEN
        // - Player is NOT viewing camera 4B
        if (player.m_UsingCamera && !player.m_RightDoorClosed) {
            bool viewing4B = m_CameraSystem && m_CameraSystem->GetActiveCamera() == "4B";
            if (!viewing4B && RollAICheck(freddy.aiLevel)) {
                freddy.currentLocation = Room::OFFICE;
                freddy.freddyOfficeTimer = 0.0f;
                if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < FREDDY_LAUGH_CHANCE) {
                    PlaySound("Laugh_Giggle_Girl_1");
                }
            }
        }
        // If door is closed, Freddy retreats
        if (player.m_RightDoorClosed) {
            freddy.currentLocation = Room::DINING_AREA;
            freddy.freddyMoveDelay = 0.0f;
        }
        return;
    }

    // Normal movement along deterministic path
    if (RollAICheck(freddy.aiLevel)) {
        Room nextRoom = GetNextFreddyRoom(freddy.currentLocation);
        if (nextRoom != freddy.currentLocation) {
            freddy.currentLocation = nextRoom;

            // Post-move delay: (1000 - 100*aiLevel) frames at 60fps
            float delayFrames = std::max(0.0f, 1000.0f - 100.0f * freddy.aiLevel);
            freddy.freddyMoveDelay = delayFrames / 60.0f;

            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < FREDDY_LAUGH_CHANCE) {
                PlaySound("Laugh_Giggle_Girl_1");
            }
        }
    }
}

Room FNAFGame::GetNextFreddyRoom(Room current) {
    for (size_t i = 0; i < FREDDY_PATH.size() - 1; ++i) {
        if (FREDDY_PATH[i] == current) {
            return FREDDY_PATH[i + 1];
        }
    }
    return current; // Already at end of path
}

// ============================================================================
// BONNIE AI - Random teleportation to left-side rooms
// ============================================================================

void FNAFGame::UpdateBonnie(Animatronic &bonnie, float deltaTime) {
    // If Bonnie is in the office, do nothing (jumpscare handled by CheckForJumpscare)
    if (bonnie.currentLocation == Room::OFFICE) return;

    // At WEST_CORNER: attempt to enter office
    if (bonnie.currentLocation == Room::WEST_CORNER) {
        bonnie.timeSinceLastMoveCheck += deltaTime;
        if (bonnie.timeSinceLastMoveCheck >= bonnie.moveInterval) {
            bonnie.timeSinceLastMoveCheck = 0.0f;
            if (RollAICheck(bonnie.aiLevel)) {
                if (!player.m_LeftDoorClosed) {
                    bonnie.currentLocation = Room::OFFICE;
                } else {
                    // Door closed - retreat to dining area
                    bonnie.currentLocation = Room::DINING_AREA;
                }
            }
        }
        return;
    }

    // Normal movement: teleport to random left-side room
    bonnie.timeSinceLastMoveCheck += deltaTime;
    if (bonnie.timeSinceLastMoveCheck >= bonnie.moveInterval) {
        bonnie.timeSinceLastMoveCheck = 0.0f;
        if (RollAICheck(bonnie.aiLevel)) {
            bonnie.currentLocation = GetRandomBonnieRoom();
        }
    }
}

Room FNAFGame::GetRandomBonnieRoom() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(BONNIE_ROOMS.size()) - 1);
    return BONNIE_ROOMS[dist(m_RNG)];
}

// ============================================================================
// CHICA AI - Adjacent room movement on right side
// ============================================================================

void FNAFGame::UpdateChica(Animatronic &chica, float deltaTime) {
    if (chica.currentLocation == Room::OFFICE) return;

    // At EAST_CORNER: attempt to enter office
    if (chica.currentLocation == Room::EAST_CORNER) {
        chica.timeSinceLastMoveCheck += deltaTime;
        if (chica.timeSinceLastMoveCheck >= chica.moveInterval) {
            chica.timeSinceLastMoveCheck = 0.0f;
            if (RollAICheck(chica.aiLevel)) {
                if (!player.m_RightDoorClosed) {
                    chica.currentLocation = Room::OFFICE;
                } else {
                    // Door closed - retreat to dining area
                    chica.currentLocation = Room::DINING_AREA;
                }
            }
        }
        return;
    }

    // Kitchen noise
    if (chica.currentLocation == Room::KITCHEN) {
        if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < 0.05f * deltaTime) {
            PlaySound("kitchen/noise" + std::to_string(std::uniform_int_distribution<int>(1, 4)(m_RNG)));
        }
    }

    // Normal movement: move to random adjacent room on right side
    chica.timeSinceLastMoveCheck += deltaTime;
    if (chica.timeSinceLastMoveCheck >= chica.moveInterval) {
        chica.timeSinceLastMoveCheck = 0.0f;
        if (RollAICheck(chica.aiLevel)) {
            chica.currentLocation = GetRandomAdjacentChicaRoom(chica.currentLocation);
        }
    }
}

Room FNAFGame::GetRandomAdjacentChicaRoom(Room current) {
    auto it = CHICA_ADJACENCY.find(current);
    if (it == CHICA_ADJACENCY.end() || it->second.empty()) {
        return current; // No adjacent rooms defined
    }
    const auto &adjacent = it->second;
    std::uniform_int_distribution<int> dist(0, static_cast<int>(adjacent.size()) - 1);
    return adjacent[dist(m_RNG)];
}

// ============================================================================
// FOXY AI - 4-stage progression with sprint attack
// ============================================================================

void FNAFGame::UpdateFoxy(Animatronic &foxy, float deltaTime) {
    // Foxy has already attacked and is waiting to reset
    if (foxy.currentLocation == Room::OFFICE) return;

    // If Foxy is sprinting (stage 3, at West Hall), handle the attack
    if (foxy.foxyStage >= 3 && foxy.currentLocation == Room::WEST_HALL) {
        // Sprint attack!
        if (!player.m_LeftDoorClosed) {
            // Door open - jumpscare
            PlaySound("foxy_run");
            TriggerJumpscare(foxy);
        } else {
            // Door closed - bang on it, drain power (real FNAF1: 1%, 6%, 11%, 16%, 21%...)
            PlaySound("door_bang");
            float powerDrain = 1.0f + foxy.foxyDoorBangCount * 5.0f;
            player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - powerDrain);
            foxy.foxyDoorBangCount++;

            // Reset Foxy back to Pirate Cove
            foxy.currentLocation = Room::PIRATE_COVE;
            foxy.foxyStage = 0;
            foxy.timeSinceLastMoveCheck = 0.0f;
        }
        return;
    }

    // Camera blocking: while ANY camera is up, Foxy auto-fails ALL movement checks
    if (player.m_UsingCamera) {
        foxy.timeSinceLastMoveCheck = 0.0f;
        return;
    }

    // After cameras come down, random cooldown before Foxy can move (0.83-16.67 seconds)
    if (m_WasCameraUp && !player.m_UsingCamera) {
        // Camera just went down - set random cooldown
        std::uniform_real_distribution<float> cooldownDist(0.83f, 16.67f);
        foxy.foxyCooldownTimer = cooldownDist(m_RNG);
    }

    if (foxy.foxyCooldownTimer > 0.0f) {
        foxy.foxyCooldownTimer -= deltaTime;
        return;
    }

    // Movement opportunity
    foxy.timeSinceLastMoveCheck += deltaTime;
    if (foxy.timeSinceLastMoveCheck >= foxy.moveInterval) {
        foxy.timeSinceLastMoveCheck = 0.0f;

        if (RollAICheck(foxy.aiLevel)) {
            foxy.foxyStage++;

            if (foxy.foxyStage >= 3) {
                // Stage 3: Foxy leaves Pirate Cove and runs to the office
                foxy.currentLocation = Room::WEST_HALL;
                // The sprint attack will be handled next frame
            }
        }
    }
}

// ============================================================================
// JUMPSCARE & DEFENSE
// ============================================================================

void FNAFGame::CheckForJumpscare() {
    for (const auto &[name, animatronic] : m_Animatronics) {
        if (animatronic->currentLocation == Room::OFFICE) {
            // Freddy has his own jumpscare timing in UpdateFreddy
            if (name == "Freddy") continue;

            if (!IsDefendedAgainst(*animatronic)) {
                TriggerJumpscare(*animatronic);
                return;
            }
        }
    }
}

bool FNAFGame::IsDefendedAgainst(const Animatronic &animatronic) const {
    if (animatronic.name == "Freddy" || animatronic.name == "Chica") {
        return player.m_RightDoorClosed;
    } else if (animatronic.name == "Bonnie" || animatronic.name == "Foxy") {
        return player.m_LeftDoorClosed;
    }
    return false;
}

void FNAFGame::TriggerJumpscare(const Animatronic &character) {
    PlaySound("JumpScare/AllAnimitronics");
    m_GameOver = true;
}

// ============================================================================
// POWER OUTAGE
// ============================================================================

void FNAFGame::HandlePowerOutage(float deltaTime) {
    m_PhaseTimer += deltaTime;
    m_PhaseCheckTimer += deltaTime;

    std::uniform_real_distribution<float> chance(0.0f, 1.0f);

    switch (m_PowerOutagePhase) {
        case PowerOutagePhase::DARK_WAIT:
            // Every 5 seconds, 20% chance to advance. Force advance at 20s.
            if (m_PhaseTimer >= 20.0f || (m_PhaseCheckTimer >= 5.0f && chance(m_RNG) < 0.2f)) {
                m_PowerOutagePhase = PowerOutagePhase::FREDDY_FACE;
                m_PhaseTimer = 0.0f;
                m_PhaseCheckTimer = 0.0f;
                PlaySound("music_box");
            }
            if (m_PhaseCheckTimer >= 5.0f) m_PhaseCheckTimer = 0.0f;
            break;

        case PowerOutagePhase::FREDDY_FACE:
            // Every 5 seconds, 20% chance to advance. Force advance at 20s.
            if (m_PhaseTimer >= 20.0f || (m_PhaseCheckTimer >= 5.0f && chance(m_RNG) < 0.2f)) {
                m_PowerOutagePhase = PowerOutagePhase::LIGHTS_OFF;
                m_PhaseTimer = 0.0f;
                m_PhaseCheckTimer = 0.0f;
            }
            if (m_PhaseCheckTimer >= 5.0f) m_PhaseCheckTimer = 0.0f;
            break;

        case PowerOutagePhase::LIGHTS_OFF:
            // Brief blackout then jumpscare
            if (m_PhaseTimer >= 0.5f) {
                m_PowerOutagePhase = PowerOutagePhase::JUMPSCARE;
                TriggerJumpscare(*m_Animatronics["Freddy"]);
            }
            break;

        case PowerOutagePhase::JUMPSCARE:
            break;
    }
}

// ============================================================================
// AI LEVELS - Real FNAF 1 starting values per night
// ============================================================================

int FNAFGame::GetAILevel(int night, const std::string &character) const {
    struct NightLevels {
        int freddy, bonnie, chica, foxy;
    };

    // Real FNAF 1 AI levels
    const std::array<NightLevels, 7> nightConfigs = {{
        {0, 0, 0, 0},     // Night 1
        {0, 3, 1, 1},     // Night 2
        {1, 0, 5, 2},     // Night 3
        {1, 2, 4, 6},     // Night 4 (Freddy is 50/50 chance of 1 or 2, using 1 for simplicity)
        {3, 5, 7, 5},     // Night 5
        {4, 10, 12, 16},  // Night 6 (Foxy was wrong at 6, should be 16)
        {5, 14, 14, 12}   // Night 7 (Default Custom Night)
    }};

    if (night < 1 || night > 7) night = 1;
    const auto &config = nightConfigs[night - 1];

    // Night 4 Freddy: 50/50 chance of 1 or 2
    if (night == 4 && character == "Freddy") {
        return std::uniform_int_distribution<int>(1, 2)(const_cast<std::mt19937&>(m_RNG));
    }

    if (character == "Freddy") return config.freddy;
    if (character == "Bonnie") return config.bonnie;
    if (character == "Chica") return config.chica;
    if (character == "Foxy") return config.foxy;

    return 0;
}

void FNAFGame::InitializeCustomNight(AILevels _AILevels) {
    auto validateLevel = [](int level) {
        return std::clamp(level, 0, MAX_AI_LEVEL);
    };

    InitializeGame(7);

    m_Animatronics["Freddy"]->aiLevel = validateLevel(_AILevels.freddy);
    m_Animatronics["Bonnie"]->aiLevel = validateLevel(_AILevels.bonnie);
    m_Animatronics["Chica"]->aiLevel = validateLevel(_AILevels.chica);
    m_Animatronics["Foxy"]->aiLevel = validateLevel(_AILevels.foxy);
}

void FNAFGame::PlaySound(const std::string &soundName) const {
    auto sound = Resources::GetMusic("Audio/" + soundName + ".wav");
    if (sound) {
        sound->play();
    }
}

bool FNAFGame::IsCameraViewingLocation(Room location) const {
    if (!m_CameraSystem || !player.m_UsingCamera) {
        return false;
    }

    std::string cameraId = m_CameraSystem->GetActiveCamera();

    const std::map<std::string, Room> cameraToRoom = {
        {"1A", Room::SHOW_STAGE},
        {"1B", Room::DINING_AREA},
        {"1C", Room::PIRATE_COVE},
        {"2A", Room::WEST_HALL},
        {"2B", Room::WEST_CORNER},
        {"3", Room::SUPPLY_CLOSET},
        {"4A", Room::EAST_HALL},
        {"4B", Room::EAST_CORNER},
        {"5", Room::KITCHEN},
        {"6", Room::RESTROOMS},
        {"7", Room::OFFICE}
    };

    auto it = cameraToRoom.find(cameraId);
    if (it != cameraToRoom.end()) {
        return it->second == location;
    }

    return false;
}
