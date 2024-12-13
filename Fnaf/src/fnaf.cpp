#include "fnaf.hpp"
#include <algorithm>
#include <iostream>
#include <thread>
#include <cmath>
#include <numeric>

#include "Assets/Resources.h"
#include "GameState.h"

namespace {
    constexpr float FREDDY_LAUGH_CHANCE = 0.1f;
    constexpr float POWER_OUTAGE_DURATION_MIN = 5.0f;
    constexpr float POWER_OUTAGE_DURATION_MAX = 30.0f;
    constexpr float FOXY_SPRINT_DURATION = 3.0f;
    constexpr float SECONDS_PER_HOUR = 89.0f;
    constexpr float BASE_SECONDS_PER_HOUR = 89.0f;
    constexpr float HOUR_PROGRESS_RATE = 1.0f / SECONDS_PER_HOUR;
}

Animatronic::Animatronic(const std::string& name, int aiLevel)
    : name(name)
    , currentLocation(Room::SHOW_STAGE)
    , aiLevel(aiLevel)
    , movementProgress(0.0f)
    , isInOffice(false)
    , isActive(true)
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
            // Regression when watched
            movementProgress = std::max(0.0f, movementProgress - delta * 0.5f);
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
    } else {
        currentLocation = Room::SHOW_STAGE;
    }
    movementProgress = 0.0f;
    isInOffice = false;
}

FNAFGame::FNAFGame()
    : m_GameOver(false)
    , m_PowerOutage(false)
    , m_PowerOutageTimer(0.0f)
    , m_FreddyMusicBoxTimer(0.0f)
    , m_RNG(std::random_device{}())
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
        // Calculate power drain rate per second
        float drainRate = CalculatePowerDrain();

        // Apply drain scaled by delta time
        player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - (drainRate * deltaTime));

        // Check for power depletion
        if (player.m_PowerLevel <= 0) {
            m_PowerOutage = true;

            // Initialize power outage sequence
            std::uniform_real_distribution<float> dist(POWER_OUTAGE_DURATION_MIN, POWER_OUTAGE_DURATION_MAX);
            m_PowerOutageTimer = dist(m_RNG);

            // Disable all systems
            player.m_UsingCamera = false;
            player.m_UsingDoor = false;
            player.m_UsingLight = false;
            std::fill(m_Doors.begin(), m_Doors.end(), false);
            std::fill(m_Lights.begin(), m_Lights.end(), false);
        }

        return;
    }

    // Handle existing power outage
    HandlePowerOutage(deltaTime);
}

float FNAFGame::CalculatePowerDrain() const {
    float drainPerSecond = BASE_POWER_DRAIN_PER_SECOND;

    // Add system multipliers (now properly scaled per second)
    if (player.m_UsingCamera) {
        drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * CAMERA_POWER_MULTIPLIER;
    }

    for (bool door : m_Doors) {
        if (door) {
            drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * DOOR_POWER_MULTIPLIER;
        }
    }

    for (bool light : m_Lights) {
        if (light) {
            drainPerSecond += BASE_POWER_DRAIN_PER_SECOND * LIGHT_POWER_MULTIPLIER;
        }
    }

    // Apply usage level multiplier
    return drainPerSecond * player.m_UsageLevel;
}

void FNAFGame::UpdateAnimatronics(float deltaTime) {
    for (auto& [name, animatronic] : m_Animatronics) {
        if (!animatronic->isActive) continue;
        
        animatronic->updateMovementProgress(deltaTime);
        
        if (ShouldAttemptMove(*animatronic)) {
            AttemptMove(*animatronic);
        }
        
        // Character-specific updates
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
}

bool FNAFGame::ShouldAttemptMove(const Animatronic& animatronic) {
    if (!animatronic.isActive) return false;
    
    auto now = std::chrono::system_clock::now();
    double timeSinceLastMove = 
        std::chrono::duration<double>(now - animatronic.lastMoved).count();
    
    double timeWindow = player.m_Night >= 3 ? ACCELERATED_TIME_WINDOW : NORMAL_TIME_WINDOW;
    
    if (timeSinceLastMove < timeWindow) return false;
    
    if (animatronic.name == "Foxy") {
        return animatronic.movementProgress >= 100.0f;
    }
    
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float baseProbability = static_cast<float>(animatronic.aiLevel) / MAX_AI_LEVEL;

    if (player.m_Time > 3) {
        baseProbability *= 1.25f;
    }

    return dist(m_RNG) < baseProbability;
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
    if (player.m_UsingCamera) {
        freddy.movementProgress = std::max(0.0f, freddy.movementProgress - deltaTime * 5.0f);
    } else {
        freddy.movementProgress += deltaTime * (freddy.aiLevel * 0.1f);
    }
    
    // Handle power outage sequence
    if (m_PowerOutage && m_FreddyMusicBoxTimer > 0.0f) {
        m_FreddyMusicBoxTimer -= deltaTime;
        if (m_FreddyMusicBoxTimer <= 0.0f) {
            TriggerJumpscare("Freddy");
        }
    }
}

void FNAFGame::UpdateBonnie(Animatronic& bonnie, float deltaTime) {
    // Bonnie is more aggressive on the left side
    if (bonnie.currentLocation == Room::WEST_CORNER) {
        bonnie.movementProgress += deltaTime * (bonnie.aiLevel * 0.15f);
    } else {
        bonnie.movementProgress += deltaTime * (bonnie.aiLevel * 0.1f);
    }
}

void FNAFGame::UpdateChica(Animatronic& chica, float deltaTime) {
    // Chica makes noise in the kitchen
    if (chica.currentLocation == Room::KITCHEN) {
        if (std::uniform_real_distribution<float>(0.0f, 1.0f)(m_RNG) < 0.1f) {
            PlaySound("kitchen/noise1");
        }
    }
    
    chica.movementProgress += deltaTime * (chica.aiLevel * 0.1f);
}

void FNAFGame::UpdateFoxy(Animatronic& foxy, float deltaTime) {
    // Foxy's sprint sequence
    if (foxy.movementProgress >= 100.0f && foxy.currentLocation == Room::PIRATE_COVE) {
        PlaySound("foxy_run");
        foxy.currentLocation = Room::WEST_HALL;
        
        // Check if left door is closed
        if (!m_Doors[0]) { // Left door
            TriggerJumpscare("Foxy");
        } else {
            // Drain extra power and reset Foxy
            player.m_PowerLevel = std::max(0.0f, player.m_PowerLevel - 15.0f);
            PlaySound("door_bang");
            foxy.reset();
        }
    }
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
        }
    }

    if (m_FreddyMusicBoxTimer > 0.0f) {
        m_FreddyMusicBoxTimer -= deltaTime;
        if (m_FreddyMusicBoxTimer <= 0.0f) {
            TriggerJumpscare("Freddy");
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
            TriggerJumpscare(animatronic.name);
        } else {
            // Retreat if blocked
            animatronic.reset();
        }
    }
}

bool FNAFGame::IsDefendedAgainst(const Animatronic& animatronic) const {
    if (animatronic.name == "Freddy") {
        return player.m_UsingCamera;
    } else if (animatronic.name == "Bonnie" || animatronic.name == "Foxy") {
        return m_Doors[0]; // Left door
    } else if (animatronic.name == "Chica") {
        return m_Doors[1]; // Right door
    }
    return false;
}

void FNAFGame::CheckForJumpscare() {
    for (const auto& [name, animatronic] : m_Animatronics) {
        if (animatronic->currentLocation == Room::OFFICE && !IsDefendedAgainst(*animatronic)) {
            TriggerJumpscare(name);
            return;
        }
    }
}

void FNAFGame::TriggerJumpscare(const std::string& character) {
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
    m_ValidMoves[Room::SHOW_STAGE] = {Room::DINING_AREA};
    m_ValidMoves[Room::DINING_AREA] = {Room::WEST_HALL, Room::EAST_HALL, Room::RESTROOMS, Room::KITCHEN};
    m_ValidMoves[Room::WEST_HALL] = {Room::WEST_CORNER, Room::SUPPLY_CLOSET};
    m_ValidMoves[Room::EAST_HALL] = {Room::EAST_CORNER, Room::KITCHEN};
    m_ValidMoves[Room::WEST_CORNER] = {Room::OFFICE};
    m_ValidMoves[Room::EAST_CORNER] = {Room::OFFICE};
    m_ValidMoves[Room::PIRATE_COVE] = {Room::WEST_HALL};
    m_ValidMoves[Room::KITCHEN] = {Room::EAST_HALL};
    m_ValidMoves[Room::RESTROOMS] = {Room::KITCHEN, Room::DINING_AREA};
}

int FNAFGame::GetAILevel(int night, const std::string& character) const {
    // Default AI levels for each night
    struct NightLevels {
        int freddy, bonnie, chica, foxy;
    };
    
    const std::array<NightLevels, 7> nightConfigs = {{
        {0, 3, 3, 1},  // Night 1
        {0, 4, 4, 2},  // Night 2
        {1, 6, 6, 4},  // Night 3
        {2, 8, 8, 6},  // Night 4
        {3, 10, 10, 8}, // Night 5
        {4, 12, 12, 10}, // Night 6
        {5, 14, 14, 12}  // Night 7 (Default Custom Night)
    }};
    
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
	sound->play();
}