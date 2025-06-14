#pragma once
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4.h"
#include "../Entity.h"
#include <vector>
#include <algorithm>

namespace Nexus
{
    class Transform
    {
    public:
        // Core transform data
        Vector3 position = Vector3::Zero;
        Quaternion rotation = Quaternion::Identity;
        Vector3 scale = Vector3::One;

        // Hierarchy support
        Entity parent = Entity::Null;
        std::vector<Entity> children;

        // Cached matrices (computed on demand)
        mutable Matrix4 localMatrix;
        mutable Matrix4 worldMatrix;
        mutable bool isDirty = true;

        // Constructors
        Transform() = default;
        Transform(const Vector3& pos) : position(pos) {}
        Transform(const Vector3& pos, const Quaternion& rot) : position(pos), rotation(rot) {}
        Transform(const Vector3& pos, const Quaternion& rot, const Vector3& scl)
            : position(pos), rotation(rot), scale(scl) {
        }

        // Matrix computation
        const Matrix4& GetLocalMatrix() const
        {
            if (isDirty)
            {
                // Compose TRS matrix: Translation * Rotation * Scale
                Matrix4 T = Matrix4::Translate(position);
                Matrix4 R = rotation.ToMatrix(); // You'll need to implement this in Quaternion
                Matrix4 S = Matrix4::Scale(scale);

                localMatrix = T * R * S;
                isDirty = false;
            }
            return localMatrix;
        }

        const Matrix4& GetWorldMatrix() const
        {
            // For now, return local matrix (hierarchy will be implemented later)
            return GetLocalMatrix();
        }

        // Transform operations
        void Translate(const Vector3& translation)
        {
            position += translation;
            MarkDirty();
        }

        void Rotate(const Quaternion& rotation)
        {
            this->rotation *= rotation;
            MarkDirty();
        }

        void SetPosition(const Vector3& newPosition)
        {
            position = newPosition;
            MarkDirty();
        }

        void SetRotation(const Quaternion& newRotation)
        {
            rotation = newRotation;
            MarkDirty();
        }

        void SetScale(const Vector3& newScale)
        {
            scale = newScale;
            MarkDirty();
        }

        // Euler angle convenience functions
        void SetEulerAngles(const Vector3& eulerAngles)
        {
            rotation = Quaternion::FromEulerAngles(eulerAngles);
            MarkDirty();
        }

        Vector3 GetEulerAngles() const
        {
            return rotation.ToEulerAngles();
        }

        // Direction vectors (based on rotation)
        Vector3 Forward() const
        {
            // Transform the forward vector (0, 0, -1) by rotation
            return rotation * Vector3::Forward;
        }

        Vector3 Right() const
        {
            return rotation * Vector3::Right;
        }

        Vector3 Up() const
        {
            return rotation * Vector3::Up;
        }

        // Hierarchy management (basic for now)
        void SetParent(Entity newParent)
        {
            // Remove from old parent's children
            if (parent.IsValid())
            {
                // TODO: Remove from parent's children list
            }

            parent = newParent;

            // Add to new parent's children
            if (parent.IsValid())
            {
                // TODO: Add to parent's children list
            }

            MarkDirty();
        }

        void AddChild(Entity child)
        {
            children.push_back(child);
        }

        void RemoveChild(Entity child)
        {
            auto it = std::find(children.begin(), children.end(), child);
            if (it != children.end())
            {
                children.erase(it);
            }
        }

        // Utility
        void MarkDirty() const
        {
            isDirty = true;
            // TODO: Mark children dirty as well when hierarchy is fully implemented
        }

        std::string ToString() const
        {
            return "Transform(pos: " + position.ToString() +
                ", rot: " + rotation.ToString() +
                ", scale: " + scale.ToString() + ")";
        }

    private:
        std::vector<Entity>::iterator find(std::vector<Entity>& vec, const Entity& entity)
        {
            return std::find(vec.begin(), vec.end(), entity);
        }
    };
}