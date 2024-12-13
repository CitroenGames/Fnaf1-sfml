#include "Animatronic.h"
#include "Player.h"
#include "AudioManager.h"
#include <random>
#include <algorithm>

Animatronic::Animatronic(Type type, Location startLocation)
    : m_Type(type)
    , m_CurrentLocation(startLocation)
    , m_AIDifficulty(0.0f)
    , m_MovementTimer(0.0f)
    , m_IsMoving(false)
{
}

void Animatronic::Init() {
    UpdateAIDifficulty();
}

void Animatronic::Update(double deltaTime) {
    if (!m_IsMoving) {
        m_MovementTimer += deltaTime;

        // Check for movement every 3.0 - 5.0 seconds
        if (m_MovementTimer >= 3.0f + (std::rand() % 20) / 10.0f) {
            m_MovementTimer = 0.0f;
            TryMove();
        }
    }
}

void Animatronic::FixedUpdate() {
    // Handle any physics-based updates if needed
}

bool Animatronic::CanMove() const {
    float chance = CalculateMovementChance();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < chance;
}

Location Animatronic::GetNextLocation() const {
    // Different movement patterns for each animatronic
    switch (m_Type) {
        case Type::BONNIE:
            // Bonnie's path: Show Stage -> Backstage -> Supply Closet -> West Hall -> West Hall Corner -> Office
            if (m_CurrentLocation == Location::SHOW_STAGE) return Location::BACKSTAGE;
            if (m_CurrentLocation == Location::BACKSTAGE) return Location::SUPPLY_CLOSET;
            if (m_CurrentLocation == Location::SUPPLY_CLOSET) return Location::WEST_HALL;
            if (m_CurrentLocation == Location::WEST_HALL) return Location::WEST_HALL_CORNER;
            if (m_CurrentLocation == Location::WEST_HALL_CORNER) return Location::OFFICE;
            break;

        case Type::CHICA:
            // Chica's path: Show Stage -> Dining Area -> Kitchen -> East Hall -> East Hall Corner -> Office
            if (m_CurrentLocation == Location::SHOW_STAGE) return Location::DINING_AREA;
            if (m_CurrentLocation == Location::DINING_AREA) return Location::KITCHEN;
            if (m_CurrentLocation == Location::KITCHEN) return Location::EAST_HALL;
            if (m_CurrentLocation == Location::EAST_HALL) return Location::EAST_HALL_CORNER;
            if (m_CurrentLocation == Location::EAST_HALL_CORNER) return Location::OFFICE;
            break;

        case Type::FREDDY: {
            // Freddy's path: Show Stage -> Dining Area -> Restrooms -> Kitchen -> East Hall -> East Hall Corner -> Office
            // Freddy only moves when cameras are off
            if (!player.m_UsingCamera) {
                if (m_CurrentLocation == Location::SHOW_STAGE) return Location::DINING_AREA;
                if (m_CurrentLocation == Location::DINING_AREA) return Location::EAST_HALL;
                if (m_CurrentLocation == Location::EAST_HALL) return Location::EAST_HALL_CORNER;
                if (m_CurrentLocation == Location::EAST_HALL_CORNER) return Location::OFFICE;
            }
            break;
        }

        case Type::FOXY: {
            // Foxy's path: Pirate Cove -> West Hall -> Office
            // Foxy moves faster if not watched frequently
            if (m_CurrentLocation == Location::PIRATE_COVE) return Location::WEST_HALL;
            if (m_CurrentLocation == Location::WEST_HALL) return Location::OFFICE;
            break;
        }
    }

    return m_CurrentLocation; // Stay in place if no valid move
}

void Animatronic::TryMove() {
    if (CanMove()) {
        Location nextLocation = GetNextLocation();
        if (IsValidMove(nextLocation)) {
            m_CurrentLocation = nextLocation;
            m_IsMoving = true;
            // Play footstep sound when moving
            AudioManager::Instance().PlayFootsteps(m_CurrentLocation);
        }
    }
}

bool Animatronic::CheckJumpscare() const {
    if (m_CurrentLocation != Location::OFFICE) {
        return false;
    }

    // Check specific conditions for each animatronic
    switch (m_Type) {
        case Type::BONNIE:
            return !player.m_UsingDoor && m_CurrentLocation == Location::WEST_HALL_CORNER;
        case Type::CHICA:
            return !player.m_UsingDoor && m_CurrentLocation == Location::EAST_HALL_CORNER;
        case Type::FREDDY:
            return !player.m_UsingCamera && m_CurrentLocation == Location::EAST_HALL_CORNER;
        case Type::FOXY:
            return m_CurrentLocation == Location::WEST_HALL;
        default:
            return false;
    }
}

float Animatronic::CalculateMovementChance() const {
    // Base chance affected by night number and AI difficulty
    float baseChance = 0.1f + (m_AIDifficulty * 0.05f);

    // Increase chance based on night number
    baseChance += (player.m_Night - 1) * 0.1f;

    // Special conditions for each animatronic
    switch (m_Type) {
        case Type::FREDDY:
            // Freddy is more active in darkness
            if (player.m_PowerLevel < 20) baseChance *= 1.5f;
            break;
        case Type::FOXY:
            // Foxy becomes more active if not watched
            if (!player.m_UsingCamera) baseChance *= 1.2f;
            break;
        default:
            break;
    }

    return std::min(baseChance, 1.0f);
}

bool Animatronic::IsValidMove(Location nextLocation) const {
    // Check if the move follows the animatronic's pattern
    switch (m_Type) {
        case Type::BONNIE:
            return (nextLocation == Location::BACKSTAGE ||
                   nextLocation == Location::SUPPLY_CLOSET ||
                   nextLocation == Location::WEST_HALL ||
                   nextLocation == Location::WEST_HALL_CORNER ||
                   nextLocation == Location::OFFICE);

        case Type::CHICA:
            return (nextLocation == Location::DINING_AREA ||
                   nextLocation == Location::KITCHEN ||
                   nextLocation == Location::EAST_HALL ||
                   nextLocation == Location::EAST_HALL_CORNER ||
                   nextLocation == Location::OFFICE);

        case Type::FREDDY:
            return (nextLocation == Location::DINING_AREA ||
                   nextLocation == Location::EAST_HALL ||
                   nextLocation == Location::EAST_HALL_CORNER ||
                   nextLocation == Location::OFFICE);

        case Type::FOXY:
            return (nextLocation == Location::WEST_HALL ||
                   nextLocation == Location::OFFICE);

        default:
            return false;
    }
}

void Animatronic::UpdateAIDifficulty() {
    // Base difficulty increases with night number
    m_AIDifficulty = static_cast<float>(player.m_Night);

    // Specific adjustments for each animatronic
    switch (m_Type) {
        case Type::BONNIE:
            m_AIDifficulty *= 1.0f;  // Standard difficulty
            break;
        case Type::CHICA:
            m_AIDifficulty *= 0.9f;  // Slightly easier
            break;
        case Type::FREDDY:
            m_AIDifficulty *= 0.7f;  // Harder to predict
            break;
        case Type::FOXY:
            m_AIDifficulty *= 1.2f;  // More aggressive
            break;
    }

    // Clamp difficulty to reasonable range
    m_AIDifficulty = std::max(1.0f, std::min(m_AIDifficulty, 20.0f));
}
