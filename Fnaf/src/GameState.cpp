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

    // Reset player state for new night
    player.ResetForNight();
    player.m_Time = 0;  // Start at 12 AM
    player.m_PowerLevel = 100.0f;
    player.m_UsageLevel = 1;
}

void GameState::Update(double deltaTime) {
    if (m_IsGameOver) {
        UpdateGameOver(deltaTime);
        return;
    }

    if (!m_IsNightComplete) {
        // Update time (89 seconds per hour)
        static double elapsedTime = 0.0;
        elapsedTime += deltaTime;

        if (elapsedTime >= 89.0) {
            elapsedTime -= 89.0;
            player.m_Time++;

            if (CheckWinCondition()) {
                EndNight(true);
            }
        }

        // Check other conditions
        CheckPower();
    }
}

bool GameState::CheckWinCondition() {
    return player.m_Time >= 6;
}

bool GameState::CheckLoseCondition() {
    return player.m_PowerLevel <= 0 || m_IsGameOver;
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

void GameState::HandleJumpscare(const Animatronic& animatronic) {
    if (!m_IsGameOver) {
        m_IsGameOver = true;
        m_JumpscareTimer = JUMPSCARE_DURATION;

        // Play jumpscare sound
        auto jumpscareSound = Resources::GetSound("Audio/Jumpscare/jumpscare.wav");
        if (jumpscareSound) {
            jumpscareSound->play();
        }

        EndNight(false);
    }
}

void GameState::UpdateGameOver(double deltaTime) {
    if (m_JumpscareTimer > 0) {
        m_JumpscareTimer -= deltaTime;
        return;
    }

    m_GameOverTimer += deltaTime;
    if (m_GameOverTimer >= GAME_OVER_DELAY) {
        SceneManager::QueueSwitchScene("Menu");
    }
}

void GameState::CheckPower() {
    if (player.m_PowerLevel <= 0 && !m_IsGameOver) {
        EndNight(false);
    }
}
