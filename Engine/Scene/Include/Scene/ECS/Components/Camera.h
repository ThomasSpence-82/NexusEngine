#pragma once
#include "Math/Matrix4.h"

namespace Nexus
{
    // Camera component wrapping existing camera functionality
    class Camera
    {
    public:
        // Camera properties
        float fov = 45.0f;          // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        // Camera type
        enum class ProjectionType
        {
            Perspective,
            Orthographic
        };

        ProjectionType projectionType = ProjectionType::Perspective;

        // Orthographic properties
        float orthoSize = 5.0f;     // Half-height of orthographic view

        // Flags
        bool isPrimary = false;     // Is this the main camera?
        bool isActive = true;       // Is camera currently active?

        // Constructors
        Camera() = default;
        Camera(float fieldOfView, float aspect, float near, float far)
            : fov(fieldOfView), aspectRatio(aspect), nearPlane(near), farPlane(far) {
        }

        // Matrix calculation
        Matrix4 GetProjectionMatrix() const
        {
            if (projectionType == ProjectionType::Perspective)
            {
                return Matrix4::Perspective(fov * 3.14159f / 180.0f, aspectRatio, nearPlane, farPlane);
            }
            else
            {
                float halfWidth = orthoSize * aspectRatio;
                float halfHeight = orthoSize;
                return Matrix4::Orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
            }
        }

        // Utility functions
        void SetPerspective(float fieldOfView, float aspect, float near, float far)
        {
            projectionType = ProjectionType::Perspective;
            fov = fieldOfView;
            aspectRatio = aspect;
            nearPlane = near;
            farPlane = far;
        }

        void SetOrthographic(float size, float aspect, float near, float far)
        {
            projectionType = ProjectionType::Orthographic;
            orthoSize = size;
            aspectRatio = aspect;
            nearPlane = near;
            farPlane = far;
        }

        std::string ToString() const
        {
            if (projectionType == ProjectionType::Perspective)
            {
                return "Camera(Perspective, FOV: " + std::to_string(fov) + "°)";
            }
            else
            {
                return "Camera(Orthographic, Size: " + std::to_string(orthoSize) + ")";
            }
        }
    };
}
