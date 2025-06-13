#pragma once

#include <cmath>
#include <string>

namespace Nexus
{
    class Vector3
    {
    public:
        float x, y, z;

        // Constructors
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float value) : x(value), y(value), z(value) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        // Basic operations
        Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
        Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
        Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
        Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

        // Assignment operations
        Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

        // Comparison
        bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
        bool operator!=(const Vector3& other) const { return !(*this == other); }

        // Math functions
        float Length() const { return std::sqrt(x * x + y * y + z * z); }
        float LengthSquared() const { return x * x + y * y + z * z; }
        
        Vector3 Normalized() const 
        { 
            float len = Length();
            return len > 0.0f ? *this / len : Vector3(); 
        }
        
        void Normalize() 
        { 
            float len = Length();
            if (len > 0.0f) { x /= len; y /= len; z /= len; }
        }

        float Dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
        
        Vector3 Cross(const Vector3& other) const 
        { 
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            ); 
        }
        
        // Utility
        std::string ToString() const 
        { 
            return "Vector3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
        }

        // Static constants
        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 Up;
        static const Vector3 Down;
        static const Vector3 Left;
        static const Vector3 Right;
        static const Vector3 Forward;
        static const Vector3 Back;
    };

    // Allow scalar * vector
    inline Vector3 operator*(float scalar, const Vector3& vector) 
    { 
        return vector * scalar; 
    }
}