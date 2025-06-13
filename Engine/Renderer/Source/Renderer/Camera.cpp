#include "Renderer/Camera.h"
#include "Core/Logger.h"
#include <cmath>

namespace Nexus
{
    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : m_Position(0.0f, 0.0f, 3.0f), m_Rotation(0.0f, 0.0f, 0.0f),
        m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();

        NEXUS_CORE_INFO("Camera created at position: " + m_Position.ToString());
    }

    void Camera::MoveForward(float distance)
    {
        Vector3 forward = GetForwardVector();
        m_Position += forward * distance;
        RecalculateViewMatrix();
    }

    void Camera::MoveRight(float distance)
    {
        Vector3 right = GetRightVector();
        m_Position += right * distance;
        RecalculateViewMatrix();
    }

    void Camera::MoveUp(float distance)
    {
        Vector3 up = GetUpVector();
        m_Position += up * distance;
        RecalculateViewMatrix();
    }

    void Camera::RotateYaw(float angle)
    {
        m_Rotation.y += angle;
        RecalculateViewMatrix();
    }

    void Camera::RotatePitch(float angle)
    {
        m_Rotation.x += angle;

        // Clamp pitch to prevent gimbal lock
        const float maxPitch = 1.5f; // ~85 degrees
        if (m_Rotation.x > maxPitch) m_Rotation.x = maxPitch;
        if (m_Rotation.x < -maxPitch) m_Rotation.x = -maxPitch;

        RecalculateViewMatrix();
    }

    void Camera::RotateRoll(float angle)
    {
        m_Rotation.z += angle;
        RecalculateViewMatrix();
    }

    Vector3 Camera::GetForwardVector() const
    {
        // Calculate forward vector from rotation
        float cosYaw = std::cos(m_Rotation.y);
        float sinYaw = std::sin(m_Rotation.y);
        float cosPitch = std::cos(m_Rotation.x);
        float sinPitch = std::sin(m_Rotation.x);

        return Vector3(
            cosYaw * cosPitch,
            sinPitch,
            sinYaw * cosPitch
        ).Normalized();
    }

    Vector3 Camera::GetRightVector() const
    {
        Vector3 forward = GetForwardVector();
        Vector3 worldUp(0.0f, 1.0f, 0.0f);
        return forward.Cross(worldUp).Normalized();
    }

    Vector3 Camera::GetUpVector() const
    {
        Vector3 forward = GetForwardVector();
        Vector3 right = GetRightVector();
        return right.Cross(forward).Normalized();
    }

    void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        m_FOV = fov;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }

    void Camera::RecalculateViewMatrix()
    {
        // Create view matrix using position and rotation
        Vector3 forward = GetForwardVector();
        Vector3 target = m_Position + forward;
        Vector3 up = GetUpVector();

        m_ViewMatrix = Matrix4::LookAt(m_Position, target, up);
    }

    void Camera::RecalculateProjectionMatrix()
    {
        // Convert FOV from degrees to radians
        float fovRadians = m_FOV * (3.14159f / 180.0f);
        m_ProjectionMatrix = Matrix4::Perspective(fovRadians, m_AspectRatio, m_NearPlane, m_FarPlane);
    }
}