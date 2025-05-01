#include "Scene.h"
#include "Graphics/LayerManager.h"


void Scene::Update(double deltaTime)
{
    m_Scene.Update(deltaTime);
}

void Scene::FixedUpdate()
{
    m_Scene.FixedUpdate();
}

void Scene::Destroy()
{
    // Clean up all entities in the scene
	//m_Scene.Clear(); // we need to implement this in the scene class

    // Clear all drawables from the layer manager
    LayerManager::Clear();
}