#pragma once
#include <cmath>
#include <string>
#include "Vector3.h"

namespace Nexus
{
    class Matrix4; // Forward declaration

    class Quaternion
    {
    public:
        float x, y, z, w;

        // Constructors
        Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Quaternion(const Vector3& axis, float angle);

        // Basic operations
        Quaternion operator+(const Quaternion& other) const { return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w); }
        Quaternion operator-(const Quaternion& other) const { return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w); }
        Quaternion operator*(const Quaternion& other) const;
        Quaternion operator*(float scalar) const { return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar); }
        Quaternion operator/(float scalar) const { return Quaternion(x / scalar, y / scalar, z / scalar, w / scalar); }

        // Vector rotation
        Vector3 operator*(const Vector3& vector) const;

        // Assignment operations
        Quaternion& operator+=(const Quaternion& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
        Quaternion& operator-=(const Quaternion& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
        Quaternion& operator*=(const Quaternion& other) { *this = *this * other; return *this; }
        Quaternion& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
        Quaternion& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

        // Math functions
        float Length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
        Quaternion Normalized() const
        {
            float len = Length();
            return len > 0.0f ? *this / len : Quaternion();
        }

        // Conversion functions
        Vector3 ToEulerAngles() const;
        Matrix4 ToMatrix() const;

        // Utility
        std::string ToString() const
        {
            return "Quaternion(" + std::to_string(x) + ", " + std::to_string(y) + ", " +
                std::to_string(z) + ", " + std::to_string(w) + ")";
        }

        // Static functions
        static Quaternion FromEulerAngles(const Vector3& eulerAngles);

        // Static constants
        static const Quaternion Identity;
    };
}