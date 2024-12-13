#include "GameState.h"
#include "Core/Window.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Scene/SceneManager.h"

void GameState::StartNight() {
    m_IsGameOver = false;
    m_IsNightComplete = false;
    m_GameOverTimer = 0.0f;
    m_JumpscareTimer = 0.0f;

    // Reset player state
    player.ResetForNight();
}

void GameState::EndNight(bool survived) {
    m_IsNightComplete = true;

    if (survived) {
        // Progress to next night
        player.m_Night++;
        OnNightCompleteDelegate.ExecuteAll();
    } else {
        m_IsGameOver = true;
        OnGameOverDelegate.ExecuteAll();
    }
}

bool GameState::CheckWinCondition() {
    // Win condition: Reach 6 AM
    return player.m_Time >= 6;
}

bool GameState::CheckLoseCondition() {
    // Lose conditions: Power depleted or jumpscare
    return player.m_PowerLevel <= 0 || m_IsGameOver;
}

void GameState::HandleJumpscare(const Animatronic& animatronic) {
    if (!m_IsGameOver) {
        m_IsGameOver = true;
        m_JumpscareTimer = JUMPSCARE_DURATION;

        // Play jumpscare animation and sound
        // TODO: Implement jumpscare effects

        EndNight(false);
    }
}

void GameState::Update(double deltaTime) {
    if (m_IsGameOver) {
        UpdateGameOver(deltaTime);
        return;
    }

    if (!m_IsNightComplete) {
        CheckTime();
        CheckPower();
        CheckAnimatronics();
    }
}

void GameState::UpdateGameOver(double deltaTime) {
    if (m_JumpscareTimer > 0) {
        m_JumpscareTimer -= deltaTime;
        return;
    }

    m_GameOverTimer += deltaTime;
    if (m_GameOverTimer >= GAME_OVER_DELAY) {
        // Return to main menu
        SceneManager::QueueSwitchScene("Menu");
    }
}

void GameState::CheckTime() {
    if (CheckWinCondition()) {
        EndNight(true);
    }
}

void GameState::CheckPower() {
    if (player.m_PowerLevel <= 0) {
        EndNight(false);
    }
}

void GameState::CheckAnimatronics() {
    // This will be called by the Animatronic class when needed
    // through HandleJumpscare
}
