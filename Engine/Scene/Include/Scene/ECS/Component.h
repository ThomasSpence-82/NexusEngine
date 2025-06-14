#pragma once
#include "Types.h"
#include "Entity.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>  // Added this include

namespace Nexus
{
    // Base component class (empty - components are just data)
    class Component
    {
    public:
        virtual ~Component() = default;
    };

    // Abstract base for component storage
    class ComponentStorageBase
    {
    public:
        virtual ~ComponentStorageBase() = default;
        virtual void RemoveComponent(EntityID entity) = 0;
        virtual bool HasComponent(EntityID entity) const = 0;
        virtual size_t GetComponentCount() const = 0;
    };

    // Templated component storage - stores components of type T
    template<typename T>
    class ComponentStorage : public ComponentStorageBase
    {
    public:
        ComponentStorage() = default;
        ~ComponentStorage() = default;

        // Add component for entity
        T& AddComponent(EntityID entity)
        {
            // Check if entity already has this component
            auto it = m_EntityToIndex.find(entity);
            if (it != m_EntityToIndex.end())
            {
                return m_Components[it->second];
            }

            // Add new component
            size_t index = m_Components.size();
            m_Components.emplace_back();
            m_Entities.push_back(entity);
            m_EntityToIndex[entity] = index;

            return m_Components.back();
        }

        // Add component with constructor arguments
        template<typename... Args>
        T& AddComponent(EntityID entity, Args&&... args)
        {
            // Check if entity already has this component
            auto it = m_EntityToIndex.find(entity);
            if (it != m_EntityToIndex.end())
            {
                return m_Components[it->second];
            }

            // Add new component with arguments
            size_t index = m_Components.size();
            m_Components.emplace_back(std::forward<Args>(args)...);
            m_Entities.push_back(entity);
            m_EntityToIndex[entity] = index;

            return m_Components.back();
        }

        // Get component for entity
        T& GetComponent(EntityID entity)
        {
            auto it = m_EntityToIndex.find(entity);
            if (it == m_EntityToIndex.end())
            {
                throw std::runtime_error("Entity does not have component");
            }
            return m_Components[it->second];
        }

        const T& GetComponent(EntityID entity) const
        {
            auto it = m_EntityToIndex.find(entity);
            if (it == m_EntityToIndex.end())
            {
                throw std::runtime_error("Entity does not have component");
            }
            return m_Components[it->second];
        }

        // Check if entity has component
        bool HasComponent(EntityID entity) const override
        {
            return m_EntityToIndex.find(entity) != m_EntityToIndex.end();
        }

        // Remove component for entity
        void RemoveComponent(EntityID entity) override
        {
            auto it = m_EntityToIndex.find(entity);
            if (it == m_EntityToIndex.end())
            {
                return; // Entity doesn't have this component
            }

            size_t indexToRemove = it->second;
            size_t lastIndex = m_Components.size() - 1;

            // If not the last element, move last element to this position
            if (indexToRemove != lastIndex)
            {
                m_Components[indexToRemove] = std::move(m_Components[lastIndex]);
                m_Entities[indexToRemove] = m_Entities[lastIndex];
                m_EntityToIndex[m_Entities[indexToRemove]] = indexToRemove;
            }

            // Remove last element
            m_Components.pop_back();
            m_Entities.pop_back();
            m_EntityToIndex.erase(entity);
        }

        // Get all components and entities (for iteration)
        const std::vector<T>& GetComponents() const { return m_Components; }
        const std::vector<EntityID>& GetEntities() const { return m_Entities; }

        size_t GetComponentCount() const override { return m_Components.size(); }

    private:
        std::vector<T> m_Components;                    // Packed array of components
        std::vector<EntityID> m_Entities;              // Parallel array of entity IDs
        std::unordered_map<EntityID, size_t> m_EntityToIndex; // Map entity to component index
    };
}