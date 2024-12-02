#pragma once

#include <nlohmann/json.hpp>

namespace Composable {
    using json = nlohmann::json;

    struct Vec3 {
        float x, y, z;
        Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
        
        Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3 operator*(float scalar) const {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }

        json Serialize() const { return json{{"x", x}, {"y", y}, {"z", z}}; }
        void Deserialize(const json& j) { x = j["x"]; y = j["y"]; z = j["z"]; }
    };
}