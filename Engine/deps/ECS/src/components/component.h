#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>

namespace Composable {
    using json = nlohmann::json;
    
    // Forward declarations
    class Node;

    class Component {
    public:
        virtual ~Component() = default;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void Update(double dt) {}
        virtual void FixedUpdate() {}
        
        virtual json Serialize() const = 0;
        virtual void Deserialize(const json& j) = 0;
        virtual std::string GetTypeName() const = 0;
        
        void SetOwner(std::weak_ptr<Node> owner) { this->owner = owner; }
        std::weak_ptr<Node> GetOwner() const { return owner; }

    protected:
        std::weak_ptr<Node> owner;
    };
}
