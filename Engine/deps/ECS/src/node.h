#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <string>
#include <functional>
#include "Components/component.h"
#include "Components/transform.h"

namespace Composable {
    class Node : public std::enable_shared_from_this<Node> {
    public:
        using NodePtr = std::shared_ptr<Node>;
        using WeakNodePtr = std::weak_ptr<Node>;
        using ComponentPtr = std::shared_ptr<Component>;

        explicit Node(const std::string& name = "Node");
        virtual ~Node() = default;

        // Hierarchy management
        void SetParent(NodePtr parent);
        void RemoveParent();
        NodePtr GetParent() const;
        const std::vector<NodePtr>& GetChildren() const;
        void AddChild(NodePtr child);
        void RemoveChild(NodePtr child);
        
        // Component management
        template<typename T, typename... Args>
        std::shared_ptr<T> AddComponent(Args&&... args) {
            auto component = std::make_shared<T>(std::forward<Args>(args)...);
            component->SetOwner(weak_from_this());
            components[std::type_index(typeid(T))] = component;
            component->OnAttach();
            return component;
        }

        template<typename T>
        void RemoveComponent() {
            auto typeIndex = std::type_index(typeid(T));
            auto it = components.find(typeIndex);
            if (it != components.end()) {
                it->second->OnDetach();
                components.erase(typeIndex);
            }
        }

        template<typename T>
        bool HasComponent() const {
            return components.find(std::type_index(typeid(T))) != components.end();
        }

        template<typename T>
        std::shared_ptr<T> GetComponent() const {
            auto it = components.find(std::type_index(typeid(T)));
            if (it != components.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

        // Node properties
        const std::string& GetName() const { return name; }
        void SetName(const std::string& newName) { name = newName; }
        bool IsActive() const { return active; }
        void SetActive(bool value);
        const std::unordered_map<std::type_index, ComponentPtr>& GetComponents() const { return components; }

        // Serialization
        json Serialize() const;
        void Deserialize(const json& j);

    private:
        std::string name;
        WeakNodePtr parent;
        std::vector<NodePtr> children;
        std::unordered_map<std::type_index, ComponentPtr> components;
        bool active = true;
    };
} // namespace Composable