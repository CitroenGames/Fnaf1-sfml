// ecs.cpp
#include "ecs.h"
#include <algorithm>

namespace ECS {

int Entity::nextId = 0;

Entity::Entity() : id(nextId++) {}

Entity::Entity(int id) : id(id) {
    nextId = std::max(nextId, id + 1);
}

int Entity::getId() const {
    return id;
}

json Entity::serialize() const {
    json j;
    j["id"] = id;
    j["components"] = json::array();
    for (const auto& [type, component] : components) {
        json componentJson;
        componentJson["type"] = component->getTypeName();
        componentJson["data"] = component->serialize();
        j["components"].push_back(componentJson);
    }
    return j;
}

void Entity::deserialize(const json& j) {
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

std::shared_ptr<Entity> World::createEntity() {
    auto entity = std::make_shared<Entity>();
    entities.push_back(entity);
    return entity;
}

void World::removeEntity(std::shared_ptr<Entity> entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

void World::addSystem(std::unique_ptr<System> system) {
    systems.push_back(std::move(system));
}

void World::update() {
    for (auto& system : systems) {
        system->update(entities);
    }
}

json World::serialize() const {
    json j;
    j["entities"] = json::array();
    for (const auto& entity : entities) {
        j["entities"].push_back(entity->serialize());
    }
    return j;
}

void World::deserialize(const json& j) {
    entities.clear();
    for (const auto& entityJson : j["entities"]) {
        auto entity = std::make_shared<Entity>();
        entity->deserialize(entityJson);
        entities.push_back(entity);
    }
}

} // namespace ECS