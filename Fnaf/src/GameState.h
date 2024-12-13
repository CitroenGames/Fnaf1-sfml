#pragma once

#include "SFML/Graphics.hpp"
#include "Composable.h"
#include "nlohmann/json.hpp"
#include "Player.h"
#include "Animatronic.h"
#include "Power.h"

class GameState : public Composable::Component {
public:
    // ECS Serialization Methods
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override { }
    std::string GetTypeName() const override { return "GameState"; }

    // Game State Management
    void StartNight();
    void EndNight(bool survived);
    bool CheckWinCondition();
    bool CheckLoseCondition();
    void HandleJumpscare(const Animatronic& animatronic);
    void Update(double deltaTime) override;

    // Delegate for game over events
    MultiCastDelegate OnGameOverDelegate;
    MultiCastDelegate OnNightCompleteDelegate;

private:
    // Game state tracking
    bool m_IsGameOver = false;
    bool m_IsNightComplete = false;
    float m_GameOverTimer = 0.0f;
    float m_JumpscareTimer = 0.0f;

    // Constants
    static constexpr float GAME_OVER_DELAY = 3.0f;
    static constexpr float JUMPSCARE_DURATION = 2.5f;

    // Helper methods
    void UpdateGameOver(double deltaTime);
    void CheckTime();
    void CheckPower();
    void CheckAnimatronics();
};
