#include "Composable.h"
#include "ExampleComponents.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace Composable;
using namespace Example;

// Helper function to print node hierarchy
void PrintNodeHierarchy(std::shared_ptr<Node> node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    auto transform = node->GetTransform();
    auto pos = transform->GetWorldPosition();
    
    std::cout << indent << "Node: " << node->GetName() 
              << " (Position: " << pos.x << ", " << pos.y << ", " << pos.z << ")" 
              << std::endl;
    
    for (const auto& child : node->GetChildren()) {
        PrintNodeHierarchy(child, depth + 1);
    }
}

// Helper function to simulate a game loop
void RunSimulation(Scene& scene, double duration) {
    using namespace std::chrono;
    auto startTime = high_resolution_clock::now();
    auto currentTime = startTime;
    
    while (true) {
        auto newTime = high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - currentTime).count();
        currentTime = newTime;
        
        // Update scene
        scene.Update(deltaTime);
        
        // Print current state
        system("cls");  // Clear console (Windows) - use "clear" for Unix
        std::cout << "Scene Hierarchy:" << std::endl;
        for (const auto& node : scene.GetRootNodes()) {
            PrintNodeHierarchy(node);
        }
        
        // Check if simulation should end
        if (currentTime - startTime > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(duration))) {
            break;
        }

        std::this_thread::sleep_for(milliseconds(16));  // ~60 FPS
    }
}

int main() {
    // Create a scene
    auto scene = std::make_unique<Scene>();
    
    // Create a central rotating platform
    auto platform = scene->CreateNode("Platform");
    platform->GetTransform()->SetLocalPosition({0, 0, 0});
    platform->AddComponent<RotatorComponent>(30.0f);  // 30 degrees per second
    platform->AddComponent<NameTagComponent>(1.0f);
    
    // Create orbiting objects
    for (int i = 0; i < 4; ++i) {
        float angle = i * (3.14159f * 0.5f);  // Spread objects in a circle
        float radius = 3.0f;
        
        auto orbiter = scene->CreateNode("Orbiter_" + std::to_string(i));
        orbiter->GetTransform()->SetLocalPosition({
            std::cos(angle) * radius,
            0.5f,
            std::sin(angle) * radius
        });
        orbiter->AddComponent<NameTagComponent>(0.5f);
        
        // Create a follower that trails behind each orbiter
        auto follower = scene->CreateNode("Follower_" + std::to_string(i));
        follower->GetTransform()->SetLocalPosition({
            std::cos(angle) * (radius + 1.0f),
            0.5f,
            std::sin(angle) * (radius + 1.0f)
        });
        auto followComponent = follower->AddComponent<FollowerComponent>();
        followComponent->SetTarget(orbiter);
        follower->AddComponent<NameTagComponent>(0.5f);
        
        // Make orbiters children of the platform so they rotate with it
        orbiter->SetParent(platform);
    }
    
    // Serialize the scene
    std::cout << "Serializing scene..." << std::endl;
    json serializedScene = scene->Serialize();
    std::cout << "Serialized JSON:" << std::endl;
    std::cout << serializedScene.dump(2) << std::endl;
    
    std::cout << "\nPress Enter to start simulation...";
    std::cin.get();
    
    // Run simulation for 10 seconds
    RunSimulation(*scene, 10.0);
    
    return 0;
}