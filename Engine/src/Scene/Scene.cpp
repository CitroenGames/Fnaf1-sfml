#include "Scene.h"
#include "Graphics/LayerManager.h"


void Scene::Update(double deltaTime) {
    m_Scene.Update(deltaTime);
}

void Scene::FixedUpdate() {
    m_Scene.FixedUpdate();
}

void Scene::Destroy() {
    LayerManager::Clear();
}
