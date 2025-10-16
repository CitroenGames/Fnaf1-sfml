#pragma once

#include "Scene.h"

class SceneManager {
public:
    static void Update(double deltaTime);

    static void FixedUpdate();

    static void Render();

    static void SwitchSceneNow(std::shared_ptr<Scene> scene);

    static void QueueSwitchScene(std::shared_ptr<Scene> scene);

    static std::shared_ptr<Scene> GetActiveScene();

    static void Destroy();

private:
    static std::shared_ptr<Scene> m_ActiveScene;
    static std::shared_ptr<Scene> m_QueuedScene;
};
