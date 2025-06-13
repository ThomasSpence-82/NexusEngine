#pragma once
#include "Types.h"
#include <string>

namespace Nexus
{
    class Registry; // Forward declaration

    class Entity
    {
    public:
        // Constructors
        Entity() : m_ID(NULL_ENTITY), m_Registry(nullptr) {}
        Entity(EntityID id, Registry* registry) : m_ID(id), m_Registry(registry) {}

        // Basic operations
        EntityID GetID() const { return m_ID; }
        bool IsValid() const { return m_ID != NULL_ENTITY && m_Registry != nullptr; }

        // Component operations (implemented in Registry.h to avoid circular dependency)
        template<typename T>
        T& AddComponent();

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template<typename T>
        T& GetComponent();

        template<typename T>
        const T& GetComponent() const;

        template<typename T>
        bool HasComponent() const;

        template<typename T>
        void RemoveComponent();

        // Utility
        std::string ToString() const
        {
            return "Entity(" + std::to_string(m_ID) + ")";
        }

        // Comparison operators
        bool operator==(const Entity& other) const { return m_ID == other.m_ID; }
        bool operator!=(const Entity& other) const { return m_ID != other.m_ID; }
        bool operator<(const Entity& other) const { return m_ID < other.m_ID; }

        // Static constants
        static const Entity Null;

    private:
        EntityID m_ID;
        Registry* m_Registry;

        friend class Registry;
    };
}
