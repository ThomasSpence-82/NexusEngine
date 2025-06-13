#pragma once

#include "Math/Vector3.h"
#include <array>
#include <string>

namespace Nexus
{
    class Matrix4
    {
    public:
        // Data stored in column-major order (OpenGL style)
        std::array<float, 16> m;

        // Constructors
        Matrix4();  // Identity matrix
        Matrix4(float diagonal);
        Matrix4(const std::array<float, 16>& values);

        // Access operators
        float& operator[](size_t index) { return m[index]; }
        const float& operator[](size_t index) const { return m[index]; }

        // Matrix operations
        Matrix4 operator*(const Matrix4& other) const;
        Vector3 operator*(const Vector3& vector) const;
        Matrix4& operator*=(const Matrix4& other);

        // Static creation functions
        static Matrix4 Identity();
        static Matrix4 Translate(const Vector3& translation);
        static Matrix4 Scale(const Vector3& scale);
        static Matrix4 RotateX(float radians);
        static Matrix4 RotateY(float radians);
        static Matrix4 RotateZ(float radians);
        static Matrix4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        static Matrix4 Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
        static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

        // Utility
        Matrix4 Transposed() const;
        std::string ToString() const;
        const float* Data() const { return m.data(); }
    };
}