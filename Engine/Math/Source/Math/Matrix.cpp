#include "Math/Matrix4.h"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace Nexus
{
    Matrix4::Matrix4()
    {
        // Identity matrix
        m.fill(0.0f);
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    Matrix4::Matrix4(float diagonal)
    {
        m.fill(0.0f);
        m[0] = m[5] = m[10] = m[15] = diagonal;
    }

    Matrix4::Matrix4(const std::array<float, 16>& values) : m(values)
    {
    }

    Matrix4 Matrix4::operator*(const Matrix4& other) const
    {
        Matrix4 result;
        
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    sum += m[row + k * 4] * other.m[k + col * 4];
                }
                result.m[row + col * 4] = sum;
            }
        }
        
        return result;
    }

    Vector3 Matrix4::operator*(const Vector3& vector) const
    {
        // Treat vector as (x, y, z, 1) for transformation
        float x = m[0] * vector.x + m[4] * vector.y + m[8] * vector.z + m[12];
        float y = m[1] * vector.x + m[5] * vector.y + m[9] * vector.z + m[13];
        float z = m[2] * vector.x + m[6] * vector.y + m[10] * vector.z + m[14];
        float w = m[3] * vector.x + m[7] * vector.y + m[11] * vector.z + m[15];
        
        // Perspective divide if needed
        if (w != 0.0f && w != 1.0f)
        {
            x /= w;
            y /= w;
            z /= w;
        }
        
        return Vector3(x, y, z);
    }

    Matrix4& Matrix4::operator*=(const Matrix4& other)
    {
        *this = *this * other;
        return *this;
    }

    Matrix4 Matrix4::Identity()
    {
        return Matrix4();
    }

    Matrix4 Matrix4::Translate(const Vector3& translation)
    {
        Matrix4 result;
        result.m[12] = translation.x;
        result.m[13] = translation.y;
        result.m[14] = translation.z;
        return result;
    }

    Matrix4 Matrix4::Scale(const Vector3& scale)
    {
        Matrix4 result;
        result.m[0] = scale.x;
        result.m[5] = scale.y;
        result.m[10] = scale.z;
        return result;
    }

    Matrix4 Matrix4::RotateX(float radians)
    {
        Matrix4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        
        result.m[5] = c;
        result.m[6] = s;
        result.m[9] = -s;
        result.m[10] = c;
        
        return result;
    }

    Matrix4 Matrix4::RotateY(float radians)
    {
        Matrix4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        
        result.m[0] = c;
        result.m[2] = -s;
        result.m[8] = s;
        result.m[10] = c;
        
        return result;
    }

    Matrix4 Matrix4::RotateZ(float radians)
    {
        Matrix4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        
        result.m[0] = c;
        result.m[1] = s;
        result.m[4] = -s;
        result.m[5] = c;
        
        return result;
    }

    Matrix4 Matrix4::Perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        Matrix4 result(0.0f);
        
        float tanHalfFov = std::tan(fov * 0.5f);
        
        result.m[0] = 1.0f / (aspectRatio * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        
        return result;
    }

    Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        Matrix4 result;
        
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (farPlane - nearPlane);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        
        return result;
    }

    Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        Vector3 f = (center - eye).Normalized();
        Vector3 s = f.Cross(up).Normalized();
        Vector3 u = s.Cross(f);
        
        Matrix4 result;
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[12] = -s.Dot(eye);
        result.m[13] = -u.Dot(eye);
        result.m[14] = f.Dot(eye);
        
        return result;
    }

    Matrix4 Matrix4::Transposed() const
    {
        Matrix4 result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                result.m[col + row * 4] = m[row + col * 4];
            }
        }
        return result;
    }

    std::string Matrix4::ToString() const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Matrix4:\n";
        for (int row = 0; row < 4; ++row)
        {
            ss << "[";
            for (int col = 0; col < 4; ++col)
            {
                ss << std::setw(8) << m[row + col * 4];
                if (col < 3) ss << ", ";
            }
            ss << "]\n";
        }
        return ss.str();
    }
}