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
    virtual json serialize() const = 0;
    virtual void deserialize(const json& j) = 0;
    virtual std::string getTypeName() const = 0;
};

class Entity {
private:
    static int nextId;
    int id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    Entity();
    explicit Entity(int id);
    int getId() const;

    template<typename T, typename... Args>
    void addComponent(Args&&... args);

    template<typename T>
    void removeComponent();

    template<typename T>
    bool hasComponent() const;

    template<typename T>
    std::shared_ptr<T> getComponent() const;

    void createComponentFromTypeName() {};

    json serialize() const;
    void deserialize(const json& j);
};

class System {
public:
    virtual void update(std::vector<std::shared_ptr<Entity>>& entities) = 0;
};

class World {
private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::unique_ptr<System>> systems;

public:
    std::shared_ptr<Entity> createEntity();
    void removeEntity(std::shared_ptr<Entity> entity);
    void addSystem(std::unique_ptr<System> system);
    void update();

    json serialize() const;
    void deserialize(const json& j);
};


// Template implementations
template<typename T, typename... Args>
void Entity::addComponent(Args&&... args) {
    components[std::type_index(typeid(T))] = std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
void Entity::removeComponent() {
    components.erase(std::type_index(typeid(T)));
}

template<typename T>
bool Entity::hasComponent() const {
    return components.find(std::type_index(typeid(T))) != components.end();
}

template<typename T>
std::shared_ptr<T> Entity::getComponent() const {
    auto it = components.find(std::type_index(typeid(T)));
    if (it != components.end()) {
        return std::static_pointer_cast<T>(it->second);
    }
    return nullptr;
}

} // namespace ECS