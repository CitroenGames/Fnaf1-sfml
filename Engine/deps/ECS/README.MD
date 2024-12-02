# Composable Framework

A lightweight, component-based composition framework for C++ applications. This framework provides a flexible architecture for building hierarchical structures with reusable components, perfect for game engines, UI systems, scene graphs, or any application requiring a tree-based composition system.

## Features

- **Component-Based Architecture**: Easily extend objects with modular functionality
- **Hierarchical Structure**: Full support for parent-child relationships
- **Transform System**: Built-in 3D transform system with local/world space handling
- **Scene Management**: Organize and manage complex hierarchies of objects
- **Serialization**: Full JSON serialization support for saving/loading scenes
- **Type-Safe**: Modern C++ design with compile-time type checking
- **Memory Safe**: Smart pointer usage throughout the framework

## Prerequisites

- C++17 or later
- CMake 3.12 or later

## Basic Usage

### Creating a Scene with Objects

```cpp
#include "composable.h"

using namespace Composable;

// Create a scene
auto scene = std::make_unique<Scene>();

// Create a parent node
auto parent = scene->CreateNode("Parent");
parent->GetTransform()->SetLocalPosition({0, 0, 0});

// Create a child node
auto child = scene->CreateNode(parent, "Child");
child->GetTransform()->SetLocalPosition({1, 0, 0});
```

### Creating Custom Components

```cpp
class CustomComponent : public Component {
public:
    void Update(double dt) override {
        // Update logic here
    }

    void OnAttach() override {
        // Called when component is attached to a node
    }

    void OnDetach() override {
        // Called when component is removed from a node
    }

    json Serialize() const override {
        json j;
        // Serialize component data
        return j;
    }

    void Deserialize(const json& j) override {
        // Deserialize component data
    }

    std::string GetTypeName() const override {
        return "CustomComponent";
    }
};

// Adding the component to a node
node->AddComponent<CustomComponent>();
```

### Scene Traversal

```cpp
// Find a node by name
auto node = scene->FindNodeByName("MyNode");

// Iterate over all nodes
scene->ForEachNode([](std::shared_ptr<Node> node) {
    // Do something with each node
});
```

### Serialization

```cpp
// Save scene to JSON
json sceneData = scene->Serialize();

// Load scene from JSON
scene->Deserialize(sceneData);
```

## Advanced Features

### Transform Hierarchy

```cpp
// Set up a transform hierarchy
parent->GetTransform()->SetLocalPosition({1, 0, 0});
child->GetTransform()->SetLocalPosition({0, 1, 0});

// Child's world position will be (1, 1, 0)
auto worldPos = child->GetTransform()->GetWorldPosition();
```

### Active State Management

```cpp
// Deactivate a node and all its children
node->SetActive(false);

// Check if a node is active
if (node->IsActive()) {
    // Node is active
}
```

## Best Practices

1. **Component Ownership**: Components should never outlive their owner nodes
2. **Transform Usage**: Always use the Transform component for spatial operations
3. **Scene Organization**: Use meaningful node names for better scene management
4. **Memory Management**: Let the framework handle node/component lifecycle

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by modern game engine architectures
- Uses [nlohmann/json](https://github.com/nlohmann/json) for serialization