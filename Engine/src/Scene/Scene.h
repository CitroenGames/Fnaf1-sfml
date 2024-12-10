#pragma once
#include "Composable.h"
#include <box2d/box2d.h>
#include <memory>

class Scene
{
public:
    Scene()
    {
        // Initialize world with default definition
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { 0.0f, 9.81f };
        m_worldId = b2CreateWorld(&worldDef);
    }

    virtual ~Scene()
    {
        if (B2_IS_NON_NULL(m_worldId))
        {
            b2DestroyWorld(m_worldId);
            m_worldId = b2_nullWorldId;
        }
    }

    virtual void Init()
    {
        // Setup physics
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = { 5.0f, 5.0f };
    }
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

    // Physics world getter
    b2WorldId GetPhysicsWorld() const { return m_worldId; }

    // Physics body creation helpers
    b2BodyId CreatePhysicsBody(const b2BodyDef& bodyDef)
    {
        return b2CreateBody(m_worldId, &bodyDef);
    }

    void DestroyPhysicsBody(b2BodyId bodyId)
    {
        if (B2_IS_NON_NULL(bodyId))
        {
            b2DestroyBody(bodyId);
        }
    }

    // Shape creation helpers
    b2ShapeId CreateBoxShape(b2BodyId bodyId, const b2Vec2& size, const b2ShapeDef& shapeDef = b2DefaultShapeDef())
    {
        b2Polygon box = b2MakeBox(size.x, size.y);
        return b2CreatePolygonShape(bodyId, &shapeDef, &box);
    }

    b2ShapeId CreateCircleShape(b2BodyId bodyId, float radius, const b2ShapeDef& shapeDef = b2DefaultShapeDef())
    {
        b2Circle circle = { 0 };
        circle.radius = radius;
        return b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }

    // Physics settings
    void SetGravity(const b2Vec2& gravity)
    {
        b2World_SetGravity(m_worldId, gravity);
    }

    b2Vec2 GetGravity() const
    {
        return b2World_GetGravity(m_worldId);
    }

protected:
    void ProcessEvents()
    {
        // Process contact events
        b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldId);
        for (int i = 0; i < contactEvents.beginCount; ++i)
        {
            HandleContactBegin(contactEvents.beginEvents[i]);
        }
        for (int i = 0; i < contactEvents.endCount; ++i)
        {
            HandleContactEnd(contactEvents.endEvents[i]);
        }
        for (int i = 0; i < contactEvents.hitCount; ++i)
        {
            HandleContactHit(contactEvents.hitEvents[i]);
        }

        // Process sensor events
        b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldId);
        for (int i = 0; i < sensorEvents.beginCount; ++i)
        {
            HandleSensorBegin(sensorEvents.beginEvents[i]);
        }
        for (int i = 0; i < sensorEvents.endCount; ++i)
        {
            HandleSensorEnd(sensorEvents.endEvents[i]);
        }
    }

    // Virtual event handlers for derived classes
    virtual void HandleContactBegin(const b2ContactBeginTouchEvent& event) {}
    virtual void HandleContactEnd(const b2ContactEndTouchEvent& event) {}
    virtual void HandleContactHit(const b2ContactHitEvent& event) {}
    virtual void HandleSensorBegin(const b2SensorBeginTouchEvent& event) {}
    virtual void HandleSensorEnd(const b2SensorEndTouchEvent& event) {}

    static constexpr float PHYSICS_TIME_STEP = 1.0f / 60.0f;
    static constexpr int32_t SUB_STEPS = 4;  // Recommended starting value for Soft Step solver

private:
    Composable::Scene m_Scene;
    b2WorldId m_worldId;
    float m_PhysicsAccumulator = 0.0f;
};