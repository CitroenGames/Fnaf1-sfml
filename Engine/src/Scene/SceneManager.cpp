#include "SceneManager.h"

#include "Audio/AudioManager.h"
#include "Core/Window.h"

namespace {
    std::shared_ptr<Scene> g_ActiveScene;
    std::shared_ptr<Scene> g_QueuedScene;
}

void SceneManager::Update(double deltaTime) {
    if (g_QueuedScene) {
        SwitchSceneNow(g_QueuedScene);
        g_QueuedScene = nullptr;
    }

    if (g_ActiveScene) {
        g_ActiveScene->Update(deltaTime);
    }
}

void SceneManager::FixedUpdate() {
    if (g_ActiveScene) {
        g_ActiveScene->FixedUpdate();
    }
}

void SceneManager::Render() {
    if (g_ActiveScene) {
        g_ActiveScene->Render();
    }
}

void SceneManager::QueueSwitchScene(std::shared_ptr<Scene> scene) {
    if (g_QueuedScene && g_QueuedScene != scene && g_QueuedScene != g_ActiveScene) {
        g_QueuedScene->Destroy();
        g_QueuedScene.reset();
    }

    g_QueuedScene = scene;
}

void SceneManager::SwitchSceneNow(std::shared_ptr<Scene> queuedScene) {
    if (g_ActiveScene == queuedScene) return;

    AudioManager::GetInstance().StopAllAudio();

    if (g_ActiveScene) {
        g_ActiveScene->Destroy();
        g_ActiveScene.reset();
    }

    g_ActiveScene = queuedScene;
    Window::UpdateViewport();

    if (g_ActiveScene) {
        g_ActiveScene->Init();
    }
}

std::shared_ptr<Scene> SceneManager::GetActiveScene() {
    return g_ActiveScene;
}

void SceneManager::Destroy() {
    if (g_QueuedScene) {
        if (g_QueuedScene != g_ActiveScene) {
            g_QueuedScene->Destroy();
        }
        g_QueuedScene.reset();
    }

    if (g_ActiveScene) {
        g_ActiveScene->Destroy();
        g_ActiveScene.reset();
    }
}
