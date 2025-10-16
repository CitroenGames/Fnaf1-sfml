#pragma once

#include "Composable.h"

class Scene {
public:
    // Scene constructor is used for loading assets not any gameplay logic.
    virtual void Init() = 0; // Initialize the scene and gameplay logic.
    virtual void Update(double deltaTime);

    virtual void FixedUpdate();

    virtual void Render() = 0;

    virtual void Destroy();

    std::shared_ptr<Composable::Node> CreateEntity(const std::string &name) {
        return m_Scene.CreateNode(name);
    }

    std::shared_ptr<Composable::Node> GetEntityByName(const std::string &name) {
        return m_Scene.FindNodeByName(name);
    }

    // Physics stuff
protected:
    static constexpr float PHYSICS_TIME_STEP = 1.0f / 60.0f;
    static constexpr int32_t SUB_STEPS = 4;
    // !Physics stuff

private:
    Composable::Scene m_Scene;
};
