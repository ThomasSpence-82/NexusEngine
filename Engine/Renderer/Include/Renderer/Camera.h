#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace Nexus
{
    class Camera
    {
    public:
        Camera(float fov = 45.0f, float aspectRatio = 16.0f/9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
        ~Camera() = default;

        // Camera controls
        void SetPosition(const Vector3& position) { m_Position = position; RecalculateViewMatrix(); }
        void SetRotation(const Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
        
        const Vector3& GetPosition() const { return m_Position; }
        const Vector3& GetRotation() const { return m_Rotation; }

        // Movement functions
        void MoveForward(float distance);
        void MoveRight(float distance);
        void MoveUp(float distance);
        
        // Rotation functions
        void RotateYaw(float angle);   // Left/Right
        void RotatePitch(float angle); // Up/Down
        void RotateRoll(float angle);  // Tilt

        // Matrix getters
        const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        Matrix4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        // Camera vectors
        Vector3 GetForwardVector() const;
        Vector3 GetRightVector() const;
        Vector3 GetUpVector() const;

        // Projection settings
        void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        
    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        Vector3 m_Position;
        Vector3 m_Rotation; // Pitch, Yaw, Roll in radians

        Matrix4 m_ViewMatrix;
        Matrix4 m_ProjectionMatrix;

        // Projection parameters
        float m_FOV;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;
    };
}