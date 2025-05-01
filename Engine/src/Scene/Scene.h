#pragma once
#include "Composable.h"
#include <memory>

class Scene
{
public:

    virtual void Init() = 0;
    virtual void Update(double deltaTime);
    virtual void FixedUpdate();
    virtual void Render() = 0;
    virtual void Destroy();

    std::shared_ptr<Composable::Node> CreateEntity(const std::string& name)
    {
        return m_Scene.CreateNode(name);
    }

    std::shared_ptr<Composable::Node> GetEntityByName(const std::string& name)
    {
        return m_Scene.FindNodeByName(name);
    }

protected:
    static constexpr float PHYSICS_TIME_STEP = 1.0f / 60.0f;
    static constexpr int32_t SUB_STEPS = 4;  // Recommended starting value for Soft Step solver

private:
    Composable::Scene m_Scene;
};