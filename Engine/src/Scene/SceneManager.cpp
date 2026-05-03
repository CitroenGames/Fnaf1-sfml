#include "SceneManager.h"

#include "Audio/AudioManager.h"

std::shared_ptr<Scene> SceneManager::m_ActiveScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_QueuedScene = nullptr;

void SceneManager::Update(double deltaTime) {
    if (m_QueuedScene) {
        SwitchSceneNow(m_QueuedScene);
        m_QueuedScene = nullptr;
    }

    if (m_ActiveScene) {
        m_ActiveScene->Update(deltaTime);
    }
}

void SceneManager::FixedUpdate() {
    if (m_ActiveScene) {
        m_ActiveScene->FixedUpdate();
    }
}

void SceneManager::Render() {
    if (m_ActiveScene) {
        m_ActiveScene->Render();
    }
}

void SceneManager::QueueSwitchScene(std::shared_ptr<Scene> scene) {
    if (m_QueuedScene && m_QueuedScene != scene && m_QueuedScene != m_ActiveScene) {
        m_QueuedScene->Destroy();
        m_QueuedScene.reset();
    }

    m_QueuedScene = scene;
}

void SceneManager::SwitchSceneNow(std::shared_ptr<Scene> queuedScene) {
    if (m_ActiveScene == queuedScene) return;

    AudioManager::GetInstance().StopAllAudio();

    if (m_ActiveScene) {
        m_ActiveScene->Destroy();
        m_ActiveScene.reset();
    }

    m_ActiveScene = queuedScene;
    if (m_ActiveScene) {
        m_ActiveScene->Init();
    }
}

std::shared_ptr<Scene> SceneManager::GetActiveScene() {
    return m_ActiveScene;
}

void SceneManager::Destroy() {
    if (m_QueuedScene) {
        if (m_QueuedScene != m_ActiveScene) {
            m_QueuedScene->Destroy();
        }
        m_QueuedScene.reset();
    }

    if (m_ActiveScene) {
        m_ActiveScene->Destroy();
        m_ActiveScene.reset();
    }
}
