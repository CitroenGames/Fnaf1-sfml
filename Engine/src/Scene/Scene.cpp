#include "Scene.h"
#include "Graphics/LayerManager.h"

void Scene::Update(double deltaTime)
{
	m_World.Update(deltaTime);
}

void Scene::FixedUpdate()
{
	m_World.FixedUpdate();
}

void Scene::Destroy()
{
	LayerManager::Clear();
}
