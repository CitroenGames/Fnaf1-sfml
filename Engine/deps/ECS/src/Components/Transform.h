#pragma once

#include "Component.h"
#include "vec3.h"

namespace Composable {
    class Transform : public Component {
    public:
        Transform();

        void SetLocalPosition(const Vec3& position);
        void SetLocalRotation(const Vec3& rotation);
        void SetLocalScale(const Vec3& scale);
        const Vec3& GetLocalPosition() const { return localPosition; }
        const Vec3& GetLocalRotation() const { return localRotation; }
        const Vec3& GetLocalScale() const { return localScale; }

        Vec3 GetWorldPosition();
        Vec3 GetWorldRotation();
        Vec3 GetWorldScale();

        void TranslateLocal(const Vec3& delta);
        void RotateLocal(const Vec3& delta);

        // Component interface
        void OnAttach() override;
        void OnDetach() override;
        json Serialize() const override;
        void Deserialize(const json& j) override;
        std::string GetTypeName() const override { return "Transform"; }
        void Update(double deltaTime) override {}
        void FixedUpdate() override {}

    private:
        Vec3 localPosition;
        Vec3 localRotation;
        Vec3 localScale;

        bool dirty;
        Vec3 worldPosition;
        Vec3 worldRotation;
        Vec3 worldScale;

        void UpdateWorldTransform();
        void MarkDirty();
    };
}