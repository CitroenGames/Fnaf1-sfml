#include "Scene.h"
#include "Layers/LayerManager.h"

void Scene::Update(double deltaTime)
{
	world.update();
}

void Scene::Destroy()
{
	LayerManager::Clear();
}
