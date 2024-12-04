#include "SceneManager.h"

std::shared_ptr<Scene> SceneManager::m_ActiveScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_QueuedScene = nullptr;

void SceneManager::Update(double deltaTime)
{
	// Switch scene if a new one is queued
	if (m_QueuedScene)
	{
		SwitchSceneNow(m_QueuedScene);
		m_QueuedScene = nullptr;
	}

	if (m_ActiveScene)
	{
		m_ActiveScene->Update(deltaTime);
	}
}

void SceneManager::FixedUpdate()
{
	if (m_ActiveScene)
	{
		m_ActiveScene->FixedUpdate();
	}
}

void SceneManager::Render()
{
	if (m_ActiveScene)
	{
		m_ActiveScene->Render();
	}
}

void SceneManager::QueueSwitchScene(std::shared_ptr<Scene> scene)
{
	m_QueuedScene = scene;
}

void SceneManager::SwitchSceneNow(std::shared_ptr<Scene> queuedScene) {
	if (m_ActiveScene)
	{
		m_ActiveScene->Destroy();
		m_ActiveScene.reset();
	}
	m_ActiveScene = queuedScene;
	if (m_ActiveScene)
	{
		m_ActiveScene->Init();
	}
}

std::shared_ptr<Scene> SceneManager::GetActiveScene()
{
	return m_ActiveScene;
}

void SceneManager::Destroy()
{
	if (m_ActiveScene)
	{
		m_ActiveScene->Destroy();
		m_ActiveScene.reset();
	}
}
