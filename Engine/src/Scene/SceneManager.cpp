#include "SceneManager.h"

Scene* SceneManager::m_ActiveScene = nullptr;
Scene* SceneManager::m_QueuedScene = nullptr;

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

void SceneManager::QueueSwitchScene(Scene* scene)
{
	m_QueuedScene = scene;
}

void SceneManager::SwitchSceneNow(Scene* scene)
{
	if (m_ActiveScene)
	{
		m_ActiveScene->Destroy();
		delete m_ActiveScene;
	}
	m_ActiveScene = scene;
	if (m_ActiveScene)
	{
		m_ActiveScene->Init();
	}
}

Scene* SceneManager::GetActiveScene()
{
	return m_ActiveScene;
}

void SceneManager::Destroy()
{
	if (m_ActiveScene)
	{
		m_ActiveScene->Destroy();
		delete m_ActiveScene;
		m_ActiveScene = nullptr;
	}
}
