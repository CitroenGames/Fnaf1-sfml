#include "Scene.h"
#include "Graphics/LayerManager.h"

void Scene::Update(double deltaTime)
{
	world.update(deltaTime);
}

void Scene::FixedUpdate()
{
	world.fixedupdate();
}

void Scene::Destroy()
{
	LayerManager::Clear();
}
