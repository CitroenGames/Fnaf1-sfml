#pragma once

#include "Scene.h"

class SceneManager
{
public:
	static void Update(double deltaTime);
	static void FixedUpdate();
	static void Render();
	static void SwitchScene(Scene* scene);
	static Scene* GetActiveScene();
	static void Destroy();

private:
	static Scene* activeScene;
};