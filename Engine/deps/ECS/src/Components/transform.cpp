#include "transform.h"
#include "node.h"

namespace Composable {

Transform::Transform()
    : localPosition(0, 0, 0)
    , localRotation(0, 0, 0)
    , localScale(1, 1, 1)
    , dirty(true)
    , worldPosition(0, 0, 0)
    , worldRotation(0, 0, 0)
    , worldScale(1, 1, 1)
{}

void Transform::OnAttach() {
    dirty = true;
    UpdateWorldTransform();
}

void Transform::OnDetach() {
    worldPosition = localPosition;
    worldRotation = localRotation;
    worldScale = localScale;
}

void Transform::SetLocalPosition(const Vec3& position) {
    localPosition = position;
    MarkDirty();
}

void Transform::SetLocalRotation(const Vec3& rotation) {
    localRotation = rotation;
    MarkDirty();
}

void Transform::SetLocalScale(const Vec3& scale) {
    localScale = scale;
    MarkDirty();
}

void Transform::TranslateLocal(const Vec3& delta) {
    SetLocalPosition(localPosition + delta);
}

void Transform::RotateLocal(const Vec3& delta) {
    SetLocalRotation(localRotation + delta);
}

void Transform::MarkDirty() {
    dirty = true;
    if (auto node = owner.lock()) {
        for (const auto& child : node->GetChildren()) {
            if (auto childTransform = child->GetComponent<Transform>()) {
                childTransform->MarkDirty();
            }
        }
    }
}

Vec3 Transform::GetWorldPosition() {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldPosition;
}

Vec3 Transform::GetWorldRotation() {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldRotation;
}

Vec3 Transform::GetWorldScale() {
    if (dirty) {
        UpdateWorldTransform();
    }
    return worldScale;
}

void Transform::UpdateWorldTransform() {
    if (auto node = owner.lock()) {
        if (auto parent = node->GetParent()) {
            if (auto parentTransform = parent->GetComponent<Transform>()) {
                auto parentWorldPos = parentTransform->GetWorldPosition();
                auto parentWorldRot = parentTransform->GetWorldRotation();
                auto parentWorldScale = parentTransform->GetWorldScale();
                
                worldPosition = parentWorldPos + (localPosition * parentWorldScale.x);
                worldRotation = parentWorldRot + localRotation;
                worldScale = parentWorldScale * localScale.x;
            } else {
                worldPosition = localPosition;
                worldRotation = localRotation;
                worldScale = localScale;
            }
        } else {
            worldPosition = localPosition;
            worldRotation = localRotation;
            worldScale = localScale;
        }
    }
    
    dirty = false;
}

json Transform::Serialize() const {
    json j;
    j["position"] = localPosition.Serialize();
    j["rotation"] = localRotation.Serialize();
    j["scale"] = localScale.Serialize();
    return j;
}

void Transform::Deserialize(const json& j) {
    localPosition.Deserialize(j["position"]);
    localRotation.Deserialize(j["rotation"]);
    localScale.Deserialize(j["scale"]);
    dirty = true;
}

} // namespace Composable