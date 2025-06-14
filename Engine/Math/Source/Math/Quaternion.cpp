#include "Math/Quaternion.h"
#include "Math/Matrix4.h"
#include <cmath>

namespace Nexus
{
    // Static constants
    const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

    // Constructor from axis-angle
    Quaternion::Quaternion(const Vector3& axis, float angle)
    {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        float cosHalf = std::cos(halfAngle);

        Vector3 normalizedAxis = axis.Normalized();

        x = normalizedAxis.x * sinHalf;
        y = normalizedAxis.y * sinHalf;
        z = normalizedAxis.z * sinHalf;
        w = cosHalf;
    }

    // Quaternion multiplication
    Quaternion Quaternion::operator*(const Quaternion& other) const
    {
        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y + y * other.w + z * other.x - x * other.z,
            w * other.z + z * other.w + x * other.y - y * other.x,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }

    // Vector rotation by quaternion
    Vector3 Quaternion::operator*(const Vector3& vector) const
    {
        // Use the formula: v' = q * v * q^-1
        // Optimized version using quaternion-vector multiplication
        Vector3 qvec(x, y, z);
        Vector3 uv = qvec.Cross(vector);
        Vector3 uuv = qvec.Cross(uv);

        return vector + ((uv * w) + uuv) * 2.0f;
    }

    // Convert to Euler angles (in radians)
    Vector3 Quaternion::ToEulerAngles() const
    {
        Vector3 angles;

        // Roll (x-axis rotation)
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);

        // Pitch (y-axis rotation)
        float sinp = 2 * (w * y - z * x);
        if (std::abs(sinp) >= 1)
            angles.y = std::copysign(3.14159265359f / 2, sinp); // Use 90 degrees if out of range
        else
            angles.y = std::asin(sinp);

        // Yaw (z-axis rotation)
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);

        return angles;
    }

    // Convert to Matrix4
    Matrix4 Quaternion::ToMatrix() const
    {
        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        return Matrix4({
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),        2.0f * (xz + wy),        0.0f,
            2.0f * (xy + wz),        1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),        0.0f,
            2.0f * (xz - wy),        2.0f * (yz + wx),        1.0f - 2.0f * (xx + yy), 0.0f,
            0.0f,                    0.0f,                    0.0f,                    1.0f
            });
    }

    // Create quaternion from Euler angles (in radians)
    Quaternion Quaternion::FromEulerAngles(const Vector3& eulerAngles)
    {
        float cx = std::cos(eulerAngles.x * 0.5f);
        float sx = std::sin(eulerAngles.x * 0.5f);
        float cy = std::cos(eulerAngles.y * 0.5f);
        float sy = std::sin(eulerAngles.y * 0.5f);
        float cz = std::cos(eulerAngles.z * 0.5f);
        float sz = std::sin(eulerAngles.z * 0.5f);

        return Quaternion(
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        );
    }
}