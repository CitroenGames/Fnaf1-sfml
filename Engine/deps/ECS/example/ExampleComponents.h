#pragma once

#include "Composable.h"
#include "Components/Transform.h"
#include <iostream>
#include <string>

namespace Example {

// A simple behavior component that rotates its node
class RotatorComponent : public Composable::Component {
public:
    explicit RotatorComponent(float speed = 45.0f) // 45 degrees per second
        : rotationSpeed(speed) {}

    void Update(double dt) override {
        if (auto node = GetOwner().lock()) {
            if (auto transform = node->GetTransform()) {
                auto rotation = transform->GetLocalRotation();
                rotation.y += rotationSpeed * static_cast<float>(dt);
                transform->SetLocalRotation(rotation);
            }
        }
    }

    void OnAttach() override {
        std::cout << "RotatorComponent attached to: " << GetOwner().lock()->GetName() << std::endl;
    }

    // Serialization
    Composable::json Serialize() const override {
        Composable::json j;
        j["speed"] = rotationSpeed;
        return j;
    }

    void Deserialize(const Composable::json& j) override {
        rotationSpeed = j["speed"];
    }

    std::string GetTypeName() const override { return "RotatorComponent"; }

private:
    float rotationSpeed;
};

// A component that maintains a name tag offset above its node
class NameTagComponent : public Composable::Component {
public:
    explicit NameTagComponent(float height = 2.0f) 
        : offsetHeight(height) {}

    void Update(double dt) override {
        if (auto node = GetOwner().lock()) {
            // Update name tag position to stay above the node
            if (auto transform = node->GetTransform()) {
                auto position = transform->GetWorldPosition();
                tagPosition = Transform::Vec3(position.x, position.y + offsetHeight, position.z);
            }
        }
    }

    const Transform::Vec3& GetTagPosition() const { return tagPosition; }
    const std::string& GetText() const { 
        return GetOwner().lock() ? GetOwner().lock()->GetName() : emptyString; 
    }

    // Serialization
    Composable::json Serialize() const override {
        Composable::json j;
        j["height"] = offsetHeight;
        return j;
    }

    void Deserialize(const Composable::json& j) override {
        offsetHeight = j["height"];
    }

    std::string GetTypeName() const override { return "NameTagComponent"; }

private:
    float offsetHeight;
    Transform::Vec3 tagPosition;
    static inline std::string emptyString;
};

// A component that follows a target node
class FollowerComponent : public Composable::Component {
public:
    void SetTarget(std::weak_ptr<Composable::Node> target) {
        this->target = target;
    }

    void Update(double dt) override {
        if (auto targetNode = target.lock()) {
            if (auto ownerNode = GetOwner().lock()) {
                auto targetTransform = targetNode->GetTransform();
                auto ownerTransform = ownerNode->GetTransform();
                
                if (targetTransform && ownerTransform) {
                    auto targetPos = targetTransform->GetWorldPosition();
                    auto currentPos = ownerTransform->GetLocalPosition();
                    
                    // Simple linear interpolation towards target
                    float speed = 2.0f * static_cast<float>(dt);
                    Transform::Vec3 newPos(
                        currentPos.x + (targetPos.x - currentPos.x) * speed,
                        currentPos.y + (targetPos.y - currentPos.y) * speed,
                        currentPos.z + (targetPos.z - currentPos.z) * speed
                    );
                    
                    ownerTransform->SetLocalPosition(newPos);
                }
            }
        }
    }

    // Serialization
    Composable::json Serialize() const override {
        return Composable::json::object();  // Nothing to serialize
    }

    void Deserialize(const Composable::json& j) override {}

    std::string GetTypeName() const override { return "FollowerComponent"; }

private:
    std::weak_ptr<Composable::Node> target;
};

} // namespace Example