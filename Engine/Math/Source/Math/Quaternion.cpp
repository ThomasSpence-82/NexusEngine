#include "Math/Quaternion.h"
#include "Math/Matrix4.h"
#include <cmath>

namespace Nexus
{
    // Static constants
    const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

    // Constructor from axis and angle
    Quaternion::Quaternion(const Vector3& axis, float angle)
    {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        Vector3 normalizedAxis = axis.Normalized();

        x = normalizedAxis.x * s;
        y = normalizedAxis.y * s;
        z = normalizedAxis.z * s;
        w = std::cos(halfAngle);
    }

    // Quaternion multiplication
    Quaternion Quaternion::operator*(const Quaternion& other) const
    {
        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }

    // Convert to Euler angles (in radians)
    Vector3 Quaternion::ToEulerAngles() const
    {
        Vector3 euler;

        // Roll (x-axis rotation)
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        euler.x = std::atan2(sinr_cosp, cosr_cosp);

        // Pitch (y-axis rotation)
        float sinp = 2 * (w * y - z * x);
        if (std::abs(sinp) >= 1)
            euler.y = std::copysign(3.14159f / 2, sinp); // Use 90 degrees if out of range
        else
            euler.y = std::asin(sinp);

        // Yaw (z-axis rotation)
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        euler.z = std::atan2(siny_cosp, cosy_cosp);

        return euler;
    }

    // Create from Euler angles (in radians)
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