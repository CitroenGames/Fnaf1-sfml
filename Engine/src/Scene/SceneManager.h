#pragma once

#include "Scene.h"

class SceneManager
{
public:
	static void Update(double deltaTime);
	static void FixedUpdate();
	static void Render();
	static void SwitchSceneNow(Scene* scene);
	static void QueueSwitchScene(Scene* scene);
	static Scene* GetActiveScene();
	static void Destroy();

private:
	static Scene* m_ActiveScene;
	static Scene* m_QueuedScene;
};