// ecs.cpp
#include "ecs.h"
#include <algorithm>

namespace ECS {

int Entity::nextId = 0;

Entity::Entity() : id(nextId++) {}

Entity::Entity(int id) : id(id) {
    nextId = std::max(nextId, id + 1);
}

int Entity::GetId() const {
    return id;
}

json Entity::Serialize() const {
    json j;
    j["id"] = id;
    j["components"] = json::array();
    for (const auto& [type, component] : components) {
        json componentJson;
        componentJson["type"] = component->GetTypeName();
        componentJson["data"] = component->Serialize();
        j["components"].push_back(componentJson);
    }
    return j;
}

void Entity::Deserialize(const json& j) {
    id = j["id"];
    //for (const auto& componentJson : j["components"]) {
    //    std::string typeName = componentJson["type"];
    //    // You need to implement a factory method to create components based on type name
    //    // This is just a placeholder
    //    auto component = createComponentFromTypeName(typeName);
    //    if (component) {
    //        component->deserialize(componentJson["data"]);
    //        components[std::type_index(typeid(*component))] = component;
    //    }
    //}
}

std::shared_ptr<Entity> World::CreateEntity() {
    auto entity = std::make_shared<Entity>();
    entities.push_back(entity);
    return entity;
}

void World::RemoveEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

void World::AddSystem(std::unique_ptr<System> system) {
    systems.push_back(std::move(system));
}

void World::Update(double deltaTime) {
    // Tick components
    for (auto& component : tickableComponents) {
        component->Update(deltaTime);
    }

    // Update systems
    for (auto& system : systems) {
        system->update(entities);
    }
}

void World::FixedUpdate()
{
    for (auto& component : tickableComponents) {
        component->FixedUpdate();
    }
}

json World::Serialize() const {
    json j;
    j["entities"] = json::array();
    for (const auto& entity : entities) {
        j["entities"].push_back(entity->Serialize());
    }
    return j;
}

void World::AddTickableComponent(std::shared_ptr<TickableComponent> component) {
    tickableComponents.push_back(component);
}

void World::RemoveTickableComponent(std::shared_ptr<TickableComponent> component) {
    tickableComponents.erase(
        std::remove(tickableComponents.begin(), tickableComponents.end(), component),
        tickableComponents.end()
    );
}

void World::Deserialize(const json& j) {
    entities.clear();
    for (const auto& entityJson : j["entities"]) {
        auto entity = std::make_shared<Entity>();
        entity->Deserialize(entityJson);
        entities.push_back(entity);
    }
}

} // namespace ECS