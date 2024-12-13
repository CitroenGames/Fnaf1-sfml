#pragma once
#include <SFML/Graphics.hpp>
#include "Composable.h"
#include "nlohmann/json.hpp"

class Animatronic : public Composable::Component {
public:
    enum class Type {
        BONNIE,
        CHICA,
        FREDDY,
        FOXY,
        GOLDEN_FREDDY
    };

    enum class Location {
        SHOW_STAGE,
        DINING_AREA,
        BACKSTAGE,
        PIRATE_COVE,
        WEST_HALL,
        EAST_HALL,
        SUPPLY_CLOSET,
        KITCHEN,
        WEST_HALL_CORNER,
        EAST_HALL_CORNER,
        OFFICE
    };

    Animatronic(Type type, Location startLocation);

    // Component Lifecycle Methods
    void Init() override;
    void Update(double deltaTime) override;
    void FixedUpdate() override;

    // Movement and AI Methods
    bool CanMove() const;
    Location GetNextLocation() const;
    void TryMove();
    bool CheckJumpscare() const;

    // Getters
    Type GetType() const { return m_Type; }
    Location GetLocation() const { return m_CurrentLocation; }
    float GetAIDifficulty() const { return m_AIDifficulty; }

    // Serialization (required by Component)
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override {}
    std::string GetTypeName() const override { return "Animatronic"; }

private:
    Type m_Type;
    Location m_CurrentLocation;
    float m_AIDifficulty;
    float m_MovementTimer;
    bool m_IsMoving;

    // AI Helper Methods
    float CalculateMovementChance() const;
    bool IsValidMove(Location nextLocation) const;
    void UpdateAIDifficulty();
};
