#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <string>
#include <nlohmann/json.hpp>

namespace ECS {

using json = nlohmann::json;

class Entity;
class Component;
class System;
class World;

class Component {
public:
    virtual ~Component() = default;
    virtual json Serialize() const = 0;
    virtual void Deserialize(const json& j) = 0;
    virtual std::string GetTypeName() const = 0;
};

class TickableComponent : public Component {
public:
    virtual void Update(double deltaTime) = 0;
    virtual void FixedUpdate() = 0;
};

class Entity {
private:
    static int nextId;
    int id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    Entity();
    explicit Entity(int id);
    int GetId() const;

    template<typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);

    template<typename T>
    void RemoveComponent();

    template<typename T>
    bool HasComponent() const;

    template<typename T>
    std::shared_ptr<T> GetComponent() const;

    void CreateComponentFromTypeName() {};

    json Serialize() const;
    void Deserialize(const json& j);
};

class System {
public:
    virtual void update(std::vector<std::shared_ptr<Entity>>& entities) = 0;
};

class World {

public:
    std::shared_ptr<Entity> CreateEntity();
    void RemoveEntity(std::shared_ptr<Entity> entity);
    void AddSystem(std::unique_ptr<System> system);
    void Update(double deltaTime);
    void FixedUpdate();
    void AddTickableComponent(std::shared_ptr<TickableComponent> component);
    void RemoveTickableComponent(std::shared_ptr<TickableComponent> component);

    json Serialize() const;
    void Deserialize(const json& j);

private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::unique_ptr<System>> systems;
    std::vector<std::shared_ptr<TickableComponent>> tickableComponents;
};

// Template implementations
template<typename T, typename... Args>
std::shared_ptr<T> Entity::AddComponent(Args&&... args) {
    auto component = std::make_shared<T>(std::forward<Args>(args)...);
    components[std::type_index(typeid(T))] = component;
    return component;
}

template<typename T>
void Entity::RemoveComponent() {
    components.erase(std::type_index(typeid(T)));
}

template<typename T>
bool Entity::HasComponent() const {
    return components.find(std::type_index(typeid(T))) != components.end();
}

template<typename T>
std::shared_ptr<T> Entity::GetComponent() const {
    auto it = components.find(std::type_index(typeid(T)));
    if (it != components.end()) {
        return std::static_pointer_cast<T>(it->second);
    }
    return nullptr;
}

} // namespace ECS