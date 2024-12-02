#include "node.h"

namespace Composable {

Node::Node(const std::string& name) : name(name) {
    AddComponent<Transform>();
}

void Node::SetParent(NodePtr newParent) {
    if (auto oldParent = parent.lock()) {
        oldParent->RemoveChild(shared_from_this());
    }
    
    if (newParent) {
        parent = newParent;
        newParent->AddChild(shared_from_this());
        
        if (auto transform = GetComponent<Transform>()) {
            transform->SetLocalPosition(transform->GetWorldPosition());
            transform->SetLocalRotation(transform->GetWorldRotation());
            transform->SetLocalScale(transform->GetWorldScale());
        }
    }
}

void Node::RemoveParent() {
    if (auto p = parent.lock()) {
        p->RemoveChild(shared_from_this());
        parent.reset();
    }
}

Node::NodePtr Node::GetParent() const {
    return parent.lock();
}

const std::vector<Node::NodePtr>& Node::GetChildren() const {
    return children;
}

void Node::AddChild(NodePtr child) {
    if (std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void Node::RemoveChild(NodePtr child) {
    children.erase(
        std::remove(children.begin(), children.end(), child),
        children.end()
    );
}

void Node::SetActive(bool value) {
    if (active != value) {
        active = value;
        for (auto& child : children) {
            child->SetActive(value);
        }
    }
}

json Node::Serialize() const {
    json j;
    j["name"] = name;
    j["active"] = active;
    
    j["components"] = json::array();
    for (const auto& [type, component] : components) {
        json componentJson;
        componentJson["type"] = component->GetTypeName();
        componentJson["data"] = component->Serialize();
        j["components"].push_back(componentJson);
    }
    
    j["children"] = json::array();
    for (const auto& child : children) {
        j["children"].push_back(child->Serialize());
    }
    
    return j;
}

void Node::Deserialize(const json& j) {
    name = j["name"];
    active = j["active"];
    
    components.clear();
    auto transform = AddComponent<Transform>();
    
    for (const auto& componentJson : j["components"]) {
        std::string typeName = componentJson["type"];
        if (typeName == "Transform") {
            transform->Deserialize(componentJson["data"]);
        }
        // Add other component types here
    }
    
    children.clear();
    for (const auto& childJson : j["children"]) {
        auto child = std::make_shared<Node>();
        child->Deserialize(childJson);
        AddChild(child);
        child->parent = weak_from_this();
    }
}

} // namespace Composable