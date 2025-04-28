#include "Scene.h"
#include "Graphics/LayerManager.h"

void Scene::Update(double deltaTime)
{
    m_Scene.Update(deltaTime);

    m_PhysicsAccumulator += static_cast<float>(deltaTime);

    while (m_PhysicsAccumulator >= PHYSICS_TIME_STEP)
    {
        //b2World_Step(m_worldId, PHYSICS_TIME_STEP, SUB_STEPS);
        ProcessEvents();
        m_PhysicsAccumulator -= PHYSICS_TIME_STEP;
    }
}

void Scene::FixedUpdate()
{
    m_Scene.FixedUpdate();
}

void Scene::Destroy()
{
    //if (B2_IS_NON_NULL(m_worldId))
    //{
    //    b2DestroyWorld(m_worldId);
    //    m_worldId = b2_nullWorldId;
    //}

    LayerManager::Clear();
}