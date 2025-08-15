#include "fnaf.hpp"

#include "Assets/Resources.h"
#include "GameState.h"
#include "CameraSystem.h"

namespace {
    constexpr float FREDDY_LAUGH_CHANCE = 0.1f;
    constexpr float POWER_OUTAGE_DURATION_MIN = 5.0f;
    constexpr float POWER_OUTAGE_DURATION_MAX = 30.0f;
    constexpr float FOXY_SPRINT_DURATION = 3.0f;
    constexpr float SECONDS_PER_HOUR = 89.0f;
    constexpr float BASE_SECONDS_PER_HOUR = 89.0f;
    constexpr float HOUR_PROGRESS_RATE = 1.0f / SECONDS_PER_HOUR;

    // FNAF 1 AI mechanics constants
    constexpr float FREDDY_MOVE_INTERVAL = 3.0f;   // 3 seconds for Freddy
    constexpr float OTHER_MOVE_INTERVAL = 4.97f;   // ~5 seconds for others

    // Hour-based AI increments - based on FNAF 1 mechanics
    const std::map<int, std::map<std::string, int>> HOUR_AI_INCREMENTS = {
        {2, {{"Bonnie", 1}, {"Chica", 0}, {"Foxy", 0}, {"Freddy", 0}}},
        {3, {{"Bonnie", 1}, {"Chica", 1}, {"Foxy", 1}, {"Freddy", 0}}},
        {4, {{"Bonnie", 1}, {"Chica", 1}, {"Foxy", 1}, {"Freddy", 0}}}
    };
}

// Initialize static member for GameEvents
std::map<GameEvent, std::vector<std::function<void()>>> GameEvents::s_Subscribers;

Animatronic::Animatronic(const std::string& name, int aiLevel)
    : name(name)
    , currentLocation(Room::SHOW_STAGE)
    , aiLevel(aiLevel)
    , movementProgress(0.0f)
    , isInOffice(false)
    , isActive(true)
    , timeSinceLastMoveCheck(0.0f)
    , moveInterval(name == "Freddy" ? FREDDY_MOVE_INTERVAL : OTHER_MOVE_INTERVAL)
{
    lastMoved = std::chrono::system_clock::now();

    // Initialize character-specific paths
    if (name == "Freddy") {
        possiblePaths = {
            {Room::SHOW_STAGE, Room::DINING_AREA, 1.0f},
            {Room::DINING_AREA, Room::RESTROOMS, 0.7f},
            {Room::RESTROOMS, Room::KITCHEN, 0.5f},
            {Room::KITCHEN, Room::EAST_HALL, 0.8f},
            {Room::EAST_HALL, Room::EAST_CORNER, 1.0f},
            {Room::EAST_CORNER, Room::OFFICE, 1.0f}
        };
    }
    else if (name == "Bonnie") {
        possiblePaths = {
            {Room::SHOW_STAGE, Room::DINING_AREA, 1.0f},
            {Room::DINING_AREA, Room::WEST_HALL, 0.8f},
            {Room::WEST_HALL, Room::SUPPLY_CLOSET, 0.3f},
            {Room::WEST_HALL, Room::WEST_CORNER, 0.7f},
            {Room::WEST_CORNER, Room::OFFICE, 1.0f}
        };
    }
    else if (name == "Chica") {
        possiblePaths = {
            {Room::SHOW_STAGE, Room::DINING_AREA, 1.0f},
            {Room::DINING_AREA, Room::KITCHEN, 0.6f},
            {Room::DINING_AREA, Room::EAST_HALL, 0.4f},
            {Room::KITCHEN, Room::EAST_HALL, 1.0f},
            {Room::EAST_HALL, Room::EAST_CORNER, 0.8f},
            {Room::EAST_CORNER, Room::OFFICE, 1.0f}
        };
    }
    else if (name == "Foxy") {
        possiblePaths = {
            {Room::PIRATE_COVE, Room::WEST_HALL, 1.0f},
            {Room::WEST_HALL, Room::OFFICE, 1.0f}
        };
        currentLocation = Room::PIRATE_COVE;
    }
}

bool Animatronic::canMove(Room destination) const {
    auto it = std::find_if(possiblePaths.begin(), possiblePaths.end(),
        [this, destination](const AnimatronicPath& path) {
            return path.from == currentLocation && path.to == destination;
        });
    return it != possiblePaths.end();
}

void Animatronic::updateMovementProgress(float delta) {
    const float PROGRESS_RATE = 0.05f; // Base rate per second

    if (name == "Foxy") {
        if (!player.m_UsingCamera) {
            // Scale progress by AI level and delta time
            movementProgress += delta * (aiLevel * PROGRESS_RATE);
        }
        else {
            // Regression when watched - commented out for now as Foxy is simplified
            // movementProgress = std::max(0.0f, movementProgress - delta * 0.5f);
        }
    }
    else {
        // Standard movement progress for other animatronics
        movementProgress += delta * (aiLevel * PROGRESS_RATE);
    }
}

void Animatronic::reset() {
    if (name == "Foxy") {
        currentLocation = Room::PIRATE_COVE;
    }
    else {
        currentLocation = Room::SHOW_STAGE;
    }
    movementProgress = 0.0f;
    isInOffice = false;
    timeSinceLastMoveCheck = 0.0f;
}

FNAFGame::FNAFGame()
    : m_GameOver(false)
    , m_PowerOutage(false)
    , m_PowerOutageTimer(0.0f)
    , m_FreddyMusicBoxTimer(0.0f)
    , m_RNG(std::random_device{}())
    , m_LastHourAIUpdated(0)
    , m_CameraSystem(nullptr)
{
    InitializeMovementPaths();
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

    // Reset player state
    player.m_UsingCamera = false;
    player.m_UsingDoor = false;
    player.m_UsingLight = false;
    player.m_LeftDoorClosed = false;
    player.m_RightDoorClosed = false;
    player.m_LeftLightOn = false;
    player.m_RightLightOn = false;

    // Set initial power usage level
    player.UpdateUsageLevel();

    // Initialize animatronics
    m_Animatronics["Freddy"] = std::make_unique<Animatronic>("Freddy", GetAILevel(night, "Freddy"));
    m_Animatronics["Bonnie"] = std::make_unique<Animatronic>("Bonnie", GetAILevel(night, "Bonnie"));
    m_Animatronics["Chica"] = std::make_unique<Animatronic>("Chica", GetAILevel(night, "Chica"));
    m_Animatronics["Foxy"] = std::make_unique<Animatronic>("Foxy", GetAILevel(night, "Foxy"));
}

void FNAFGame::Update(float deltaTime) {
    if (m_GameOver) return;

    // Cap maximum time step
    float clampedDelta = std::min(deltaTime, MAX_DELTA_TIME);

    // Update power first
    UpdatePower(clampedDelta);

    // Calculate hour progress with time acceleration
    float acceleratedDelta = clampedDelta * std::pow(TIME_ACCELERATION_FACTOR, player.m_Time);
    m_TimeProgress += acceleratedDelta / m_CurrentHourDuration;

    // Check for hour completion
    if (m_TimeProgress >= 1.0f) {
        m_TimeProgress -= 1.0f;
        player.m_Time++;

        // Update hour duration with acceleration
        m_CurrentHourDuration *= TIME_ACCELERATION_FACTOR;

        if (player.m_Time >= 6) {
            m_GameOver = true;
            return;
        }
    }

    // Update animatronics with proper time scaling
    UpdateAnimatronics(clampedDelta);
    CheckForJumpscare();
}

void FNAFGame::UpdatePower(float deltaTime) {
    // Only update power if we're not already in a power outage
    if (!m_PowerOutage) {
        // Calculate power drain rate per second using new system
        float drainRate = CalculatePowerDrain();

        // Apply drain scaled by delta time
        player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - (drainRate * deltaTime));

        // Check for power depletion
        if (player.m_PowerLevel <= 0) {
            m_PowerOutage = true;

            // Initialize power outage sequence
            std::uniform_real_distribution<float> dist(POWER_OUTAGE_DURATION_MIN, POWER_OUTAGE_DURATION_MAX);
            m_PowerOutageTimer = dist(m_RNG);

            // Disable all systems and update player state
            player.m_UsingCamera = false;
            player.m_UsingDoor = false;
            player.m_UsingLight = false;
            player.m_LeftDoorClosed = false;
            player.m_RightDoorClosed = false;
            player.m_LeftLightOn = false;
            player.m_RightLightOn = false;
            player.UpdateUsageLevel();

            // Notify other systems about power outage
            GameEvents::TriggerEvent(GameEvent::POWER_OUTAGE);
        }

        return;
    }

    // Handle existing power outage
    HandlePowerOutage(deltaTime);
}

float FNAFGame::CalculatePowerDrain() const {
    float drainPerSecond = BASE_POWER_DRAIN_PER_SECOND;

    // Add system multipliers using the new centralized state
    if (player.m_UsingCamera) {
        drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * CAMERA_POWER_MULTIPLIER;
    }

    if (player.m_LeftDoorClosed) {
        drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * DOOR_POWER_MULTIPLIER;
    }

    if (player.m_RightDoorClosed) {
        drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * DOOR_POWER_MULTIPLIER;
    }

    if (player.m_LeftLightOn || player.m_RightLightOn) {
        drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * LIGHT_POWER_MULTIPLIER;
    }

    // Apply usage level multiplier
    return drainPerSecond * player.m_UsageLevel;
}

void FNAFGame::UpdateAnimatronics(float deltaTime) {
    for (auto& [name, animatronic] : m_Animatronics) {
        if (!animatronic->isActive) continue;

        // Update movement check timer
        animatronic->timeSinceLastMoveCheck += deltaTime;

        // Check for hour-based AI increments
        if (player.m_Time > m_LastHourAIUpdated) {
            auto hourIncrements = HOUR_AI_INCREMENTS.find(player.m_Time);
            if (hourIncrements != HOUR_AI_INCREMENTS.end()) {
                auto nameIncrements = hourIncrements->second.find(name);
                if (nameIncrements != hourIncrements->second.end()) {
                    animatronic->aiLevel += nameIncrements->second;
                    // Cap at maximum AI level
                    animatronic->aiLevel = std::min(animatronic->aiLevel, MAX_AI_LEVEL);
                }
            }
            m_LastHourAIUpdated = player.m_Time;
        }

        // Character-specific updates
        if (name == "Freddy") {
            UpdateFreddy(*animatronic, deltaTime);
        }
        else if (name == "Bonnie") {
            UpdateBonnie(*animatronic, deltaTime);
        }
        else if (name == "Chica") {
            UpdateChica(*animatronic, deltaTime);
        }
        else if (name == "Foxy") {
            UpdateFoxy(*animatronic, deltaTime);
        }

        // Now check if animatronic should attempt to move
        if (ShouldAttemptMove(*animatronic)) {
            AttemptMove(*animatronic);
        }
    }
}

bool FNAFGame::ShouldAttemptMove(const Animatronic& animatronic) {
    if (!animatronic.isActive) return false;

    // Check if it's time for a movement opportunity
    if (animatronic.timeSinceLastMoveCheck < animatronic.moveInterval) {
        return false;
    }

    // Reset timer
    animatronic.timeSinceLastMoveCheck = 0.0f;

    // For Foxy, we'll handle movement differently - this is simplified
    if (animatronic.name == "Foxy") {
        // Simplified Foxy logic - just use AI level as direct chance
        std::uniform_int_distribution<int> dist(1, MAX_RANDOM_ROLL);
        return dist(m_RNG) <= animatronic.aiLevel;

        /* Original complex logic commented out
        return animatronic.movementProgress >= 100.0f;
        */
    }

    // For other animatronics, generate random 1-20 roll
    std::uniform_int_distribution<int> dist(1, MAX_RANDOM_ROLL);
    int roll = dist(m_RNG);

    // If roll <= AI level, the animatronic moves
    return roll <= animatronic.aiLevel;
}

void FNAFGame::AttemptMove(Animatronic& animatronic) {
    Room nextRoom = GetNextRoom(animatronic);

    if (animatronic.canMove(nextRoom)) {
        MoveAnimatronic(animatronic, nextRoom);

        // Special effects
        if (animatronic.name == "Freddy" &&
            nextRoom != Room::OFFICE) {
            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < FREDDY_LAUGH_CHANCE) {
                PlaySound("freddy_laugh");
            }
        }

        animatronic.lastMoved = std::chrono::system_clock::now();
    }
}

void FNAFGame::UpdateFreddy(Animatronic& freddy, float deltaTime) {
    // Freddy only moves when cameras are down
    if (player.m_UsingCamera && freddy.currentLocation != Room::OFFICE) {
        // Freddy gets "stalled" by camera check
        freddy.timeSinceLastMoveCheck = 0.0f;
    }

    // Handle "bounce back" if looking at Freddy with camera
    if (player.m_UsingCamera && m_CameraSystem) {
        // If Freddy is in East Hall Corner and player is viewing that camera
        if (freddy.currentLocation == Room::EAST_CORNER &&
            m_CameraSystem->GetActiveCamera() == "4B") {
            // Stall Freddy's movement
            freddy.timeSinceLastMoveCheck = 0.0f;
        }
    }

    // Handle power outage sequence
    if (m_PowerOutage && m_FreddyMusicBoxTimer > 0.0f) {
        m_FreddyMusicBoxTimer -= deltaTime;
        if (m_FreddyMusicBoxTimer <= 0.0f) {
            TriggerJumpscare(freddy);
        }
    }
}

void FNAFGame::UpdateBonnie(Animatronic& bonnie, float deltaTime) {
    // Bonnie movement is fairly standard - mainly handled by core AI
    // No camera stalling effect
}

void FNAFGame::UpdateChica(Animatronic& chica, float deltaTime) {
    // Chica makes noise in the kitchen
    if (chica.currentLocation == Room::KITCHEN) {
        if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < 0.05f) {
            PlaySound("kitchen/noise" + std::to_string(std::uniform_int_distribution<int>(1, 3)(m_RNG)));
        }
    }
}

void FNAFGame::UpdateFoxy(Animatronic& foxy, float deltaTime) {
    // Simplified Foxy behavior - movement based on FNAF 1 mechanics
    if (player.m_UsingCamera && m_CameraSystem) {
        // Check if player is viewing Pirate Cove
        bool viewingPirateCove = m_CameraSystem->GetActiveCamera() == "1C";

        if (viewingPirateCove) {
            // Reset Foxy timer if watched
            foxy.timeSinceLastMoveCheck = 0.0f;
        }
    }

    /* Complex Foxy sprint sequence commented out
    if (foxy.movementProgress >= 100.0f && foxy.currentLocation == Room::PIRATE_COVE) {
        PlaySound("foxy_run");
        foxy.currentLocation = Room::WEST_HALL;

        // Check if left door is closed
        if (!player.m_LeftDoorClosed) { // Left door
            TriggerJumpscare("Foxy");
        } else {
            // Drain extra power and reset Foxy
            player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - 15.0f);
            PlaySound("door_bang");
            foxy.reset();
        }
    }
    */
}

void FNAFGame::HandlePowerOutage(float deltaTime) {
    if (m_PowerOutageTimer > 0.0f) {
        m_PowerOutageTimer -= deltaTime;

        if (m_PowerOutageTimer <= 0.0f) {
            PlaySound("music_box");

            // Adjust Freddy's music box duration based on night difficulty
            float baseDuration = 10.0f;
            float difficultyMultiplier = 1.0f - (player.m_Night * 0.1f);
            std::uniform_real_distribution<float> dist(
                baseDuration * difficultyMultiplier * 0.8f,
                baseDuration * difficultyMultiplier * 1.2f
            );
            m_FreddyMusicBoxTimer = dist(m_RNG);

            // Notify other systems about power outage progression
            GameEvents::TriggerEvent(GameEvent::POWER_OUTAGE);
        }
    }

    if (m_FreddyMusicBoxTimer > 0.0f) {
        m_FreddyMusicBoxTimer -= deltaTime;
        if (m_FreddyMusicBoxTimer <= 0.0f) {
            // Freddy jumpscare if music box runs out
            TriggerJumpscare(*m_Animatronics["Freddy"]);
        }
    }
}

Room FNAFGame::GetNextRoom(const Animatronic& animatronic) {
    std::vector<Room> possibleRooms;
    std::vector<float> probabilities;

    for (const auto& path : animatronic.possiblePaths) {
        if (path.from == animatronic.currentLocation) {
            possibleRooms.push_back(path.to);
            probabilities.push_back(path.probability);
        }
    }

    if (possibleRooms.empty()) {
        return animatronic.currentLocation;
    }

    // Normalize probabilities
    float sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
    for (float& prob : probabilities) {
        prob /= sum;
    }

    // Select room based on probabilities
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float random = dist(m_RNG);
    float cumulative = 0.0f;

    for (size_t i = 0; i < possibleRooms.size(); ++i) {
        cumulative += probabilities[i];
        if (random <= cumulative) {
            return possibleRooms[i];
        }
    }

    return possibleRooms.back();
}

void FNAFGame::MoveAnimatronic(Animatronic& animatronic, Room destination) {
    if (!animatronic.canMove(destination)) return;

    animatronic.currentLocation = destination;
    animatronic.movementProgress = 0.0f;

    // Check if reached office
    if (destination == Room::OFFICE) {
        if (!IsDefendedAgainst(animatronic)) {
            TriggerJumpscare(animatronic);
        }
        else {
            // Retreat if blocked
            animatronic.reset();
        }
    }

    // Special case for Freddy - laugh when he moves
    if (animatronic.name == "Freddy" && std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < 0.3f) {
        PlaySound("freddy_laugh");
    }
}

bool FNAFGame::IsDefendedAgainst(const Animatronic& animatronic) const {
    if (animatronic.name == "Freddy" || animatronic.name == "Chica") {
        return player.m_RightDoorClosed; // Right door
    }
    else if (animatronic.name == "Bonnie" || animatronic.name == "Foxy") {
        return player.m_LeftDoorClosed; // Left door
    }
    return false;
}

void FNAFGame::CheckForJumpscare() {
    for (const auto& [name, animatronic] : m_Animatronics) {
        if (animatronic->currentLocation == Room::OFFICE && !IsDefendedAgainst(*animatronic)) {
            TriggerJumpscare(*animatronic);
            return;
        }
    }
}

void FNAFGame::TriggerJumpscare(const Animatronic& character) {
    PlaySound("JumpScare/XSCREAM");
    m_GameOver = true;
}

void FNAFGame::InitializeCustomNight(AILevels _AILevels) {
    // Validate AI levels
    auto validateLevel = [](int level) {
        return std::clamp(level, 0, MAX_AI_LEVEL);
        };

    InitializeGame(7); // Custom night is night 7

    m_Animatronics["Freddy"]->aiLevel = validateLevel(_AILevels.freddy);
    m_Animatronics["Bonnie"]->aiLevel = validateLevel(_AILevels.bonnie);
    m_Animatronics["Chica"]->aiLevel = validateLevel(_AILevels.chica);
    m_Animatronics["Foxy"]->aiLevel = validateLevel(_AILevels.foxy);
}

void FNAFGame::InitializeMovementPaths() {
    // Initialize valid movement paths for each room
    m_ValidMoves[Room::SHOW_STAGE] = { Room::DINING_AREA };
    m_ValidMoves[Room::DINING_AREA] = { Room::WEST_HALL, Room::EAST_HALL, Room::RESTROOMS, Room::KITCHEN };
    m_ValidMoves[Room::WEST_HALL] = { Room::WEST_CORNER, Room::SUPPLY_CLOSET };
    m_ValidMoves[Room::EAST_HALL] = { Room::EAST_CORNER, Room::KITCHEN };
    m_ValidMoves[Room::WEST_CORNER] = { Room::OFFICE };
    m_ValidMoves[Room::EAST_CORNER] = { Room::OFFICE };
    m_ValidMoves[Room::PIRATE_COVE] = { Room::WEST_HALL };
    m_ValidMoves[Room::KITCHEN] = { Room::EAST_HALL };
    m_ValidMoves[Room::RESTROOMS] = { Room::KITCHEN, Room::DINING_AREA };
}

int FNAFGame::GetAILevel(int night, const std::string& character) const {
    // Default AI levels for each night based on FNAF 1 mechanics
    struct NightLevels {
        int freddy, bonnie, chica, foxy;
    };

    // Updated AI levels to match FNAF 1 mechanics
    const std::array<NightLevels, 7> nightConfigs = { {
        {0, 0, 0, 0},  // Night 1
        {0, 3, 1, 1},  // Night 2
        {1, 0, 5, 2},  // Night 3
        {1, 2, 4, 6},  // Night 4 (Freddy should have 50/50 chance to be 1 or 2, using 1 for simplicity)
        {3, 5, 7, 5},  // Night 5
        {4, 10, 12, 6}, // Night 6
        {5, 14, 14, 12}  // Night 7 (Default Custom Night)
    } };

    if (night < 1 || night > 7) night = 1;

    const auto& config = nightConfigs[night - 1];

    if (character == "Freddy") return config.freddy;
    if (character == "Bonnie") return config.bonnie;
    if (character == "Chica") return config.chica;
    if (character == "Foxy") return config.foxy;

    return 0;
}

void FNAFGame::PlaySound(const std::string& soundName) const {
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

    // Mapping of cameras to rooms
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