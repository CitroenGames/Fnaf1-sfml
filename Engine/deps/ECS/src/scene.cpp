#include "scene.h"
#include <algorithm>

namespace Composable {

Scene::Scene() {}

Scene::NodePtr Scene::CreateNode(const std::string& name) {
    auto node = std::make_shared<Node>(name);
    rootNodes.push_back(node);
    return node;
}

Scene::NodePtr Scene::CreateNode(NodePtr parent, const std::string& name) {
    auto node = std::make_shared<Node>(name);
    if (parent) {
        node->SetParent(parent);
    } else {
        rootNodes.push_back(node);
    }
    return node;
}

void Scene::RemoveNode(NodePtr node) {
    rootNodes.erase(
        std::remove(rootNodes.begin(), rootNodes.end(), node),
        rootNodes.end()
    );
    
    if (auto parent = node->GetParent()) {
        parent->RemoveChild(node);
    }
    
    auto children = node->GetChildren();
    for (auto& child : children) {
        RemoveNode(child);
    }
}

void Scene::ForEachNode(const std::function<void(NodePtr)>& fn) {
    std::vector<NodePtr> allNodes;
    CollectAllNodes(allNodes);
    for (auto& node : allNodes) {
        fn(node);
    }
}

Scene::NodePtr Scene::FindNodeByName(const std::string& name) {
    NodePtr result;
    ForEachNode([&](NodePtr node) {
        if (node->GetName() == name) {
            result = node;
        }
    });
    return result;
}

void Scene::Update(double deltaTime) {
    ForEachNode([deltaTime](NodePtr node) {
        if (node->IsActive()) {
            for (const auto& [type, component] : node->GetComponents()) {
                component->Update(deltaTime);
            }
        }
    });
}

void Scene::FixedUpdate() {
    ForEachNode([](NodePtr node) {
        if (node->IsActive()) {
            for (const auto& [type, component] : node->GetComponents()) {
                component->FixedUpdate();
            }
        }
    });
}

void Scene::CollectAllNodes(std::vector<NodePtr>& result) const {
    std::function<void(const NodePtr&)> collect;
    collect = [&](const NodePtr& node) {
        result.push_back(node);
        for (const auto& child : node->GetChildren()) {
            collect(child);
        }
    };
    
    for (const auto& node : rootNodes) {
        collect(node);
    }
}

json Scene::Serialize() const {
    json j;
    j["rootNodes"] = json::array();
    for (const auto& node : rootNodes) {
        j["rootNodes"].push_back(node->Serialize());
    }
    return j;
}

void Scene::Deserialize(const json& j) {
    rootNodes.clear();
    for (const auto& nodeJson : j["rootNodes"]) {
        auto node = std::make_shared<Node>();
        node->Deserialize(nodeJson);
        rootNodes.push_back(node);
    }
}

} // namespace Composable