#pragma once

#include <cmath>
#include <string>

namespace Nexus
{
    class Vector2
    {
    public:
        float x, y;

        // Constructors
        Vector2() : x(0.0f), y(0.0f) {}
        Vector2(float value) : x(value), y(value) {}
        Vector2(float x, float y) : x(x), y(y) {}

        // Basic operations
        Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
        Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
        Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

        // Assignment operations
        Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
        Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

        // Comparison
        bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
        bool operator!=(const Vector2& other) const { return !(*this == other); }

        // Math functions
        float Length() const { return std::sqrt(x * x + y * y); }
        float LengthSquared() const { return x * x + y * y; }
        
        Vector2 Normalized() const 
        { 
            float len = Length();
            return len > 0.0f ? *this / len : Vector2(); 
        }
        
        void Normalize() 
        { 
            float len = Length();
            if (len > 0.0f) { x /= len; y /= len; }
        }

        float Dot(const Vector2& other) const { return x * other.x + y * other.y; }
        
        // Utility
        std::string ToString() const 
        { 
            return "Vector2(" + std::to_string(x) + ", " + std::to_string(y) + ")"; 
        }

        // Static constants
        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 Up;
        static const Vector2 Down;
        static const Vector2 Left;
        static const Vector2 Right;
    };

    // Allow scalar * vector
    inline Vector2 operator*(float scalar, const Vector2& vector) 
    { 
        return vector * scalar; 
    }
}