#include "SceneManager.h"

Scene* SceneManager::activeScene = nullptr;

void SceneManager::Update(double deltaTime)
{
	if (activeScene)
	{
		activeScene->Update(deltaTime);
	}
}

void SceneManager::FixedUpdate()
{
	if (activeScene)
	{
		activeScene->FixedUpdate();
	}
}

void SceneManager::Render()
{
	if (activeScene)
	{
		activeScene->Render();
	}
}

void SceneManager::SwitchScene(Scene* scene)
{
	Destroy();
	activeScene = scene;
	activeScene->Init();
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}

void SceneManager::Destroy()
{
	if (activeScene)
	{
		activeScene->Destroy();
		delete activeScene;
		activeScene = nullptr;
	}
}
