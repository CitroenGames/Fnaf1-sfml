#pragma once

#include <iostream>
#include <cmath>

class Vector2 {
public:
    // Member variables
    float x;
    float y;

    // Constructors
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    // Basic operations
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const {
        if (scalar != 0) {
            return Vector2(x / scalar, y / scalar);
        }
        std::cerr << "Division by zero error!" << std::endl;
        return *this; // Handle as needed
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        if (scalar != 0) {
            x /= scalar;
            y /= scalar;
        } else {
            std::cerr << "Division by zero error!" << std::endl;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector2& other) const {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    // Dot product
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Magnitude
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Magnitude squared (useful for comparison)
    float magnitudeSquared() const {
        return x * x + y * y;
    }

    // Normalize the vector
    Vector2 normalized() const {
        float mag = magnitude();
        if (mag != 0) {
            return *this / mag;
        }
        return *this; // Return the original vector if magnitude is zero
    }

    // Utility functions
    float distanceTo(const Vector2& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }

    float distanceSquaredTo(const Vector2& other) const {
        return std::pow(x - other.x, 2) + std::pow(y - other.y, 2);
    }

    // Display vector
    void print() const {
        std::cout << "Vector2(" << x << ", " << y << ")" << std::endl;
    }

    // Friend functions for I/O
    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        os << "Vector2(" << v.x << ", " << v.y << ")";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Vector2& v) {
        is >> v.x >> v.y;
        return is;
    }
};