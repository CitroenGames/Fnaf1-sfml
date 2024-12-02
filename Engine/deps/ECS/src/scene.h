#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <node.h>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Composable {
    class Scene {
    public:
        using NodePtr = std::shared_ptr<Node>;

        Scene();
        virtual ~Scene() = default;

        // Node management
        NodePtr CreateNode(const std::string& name = "Node");
        NodePtr CreateNode(NodePtr parent, const std::string& name = "Node");
        void RemoveNode(NodePtr node);

        // Scene traversal
        void ForEachNode(const std::function<void(NodePtr)>& fn);
        NodePtr FindNodeByName(const std::string& name);

        // Scene lifecycle
        virtual void OnLoad() {}
        virtual void OnUnload() {}
        void Update(double deltaTime);
        void FixedUpdate();

        // Serialization
        json Serialize() const;
        void Deserialize(const json& j);

        const std::vector<NodePtr>& GetRootNodes() const { return rootNodes; }

    private:
        std::vector<NodePtr> rootNodes;
        void CollectAllNodes(std::vector<NodePtr>& result) const;
    };
}