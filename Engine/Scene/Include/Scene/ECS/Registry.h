#pragma once
#include "Types.h"
#include "Entity.h"
#include "Component.h"
#include <unordered_map>
#include <queue>
#include <memory>
#include <vector>

namespace Nexus
{
    class Registry
    {
    public:
        Registry() : m_NextEntityID(1) {} // Start from 1, 0 is NULL_ENTITY
        ~Registry() = default;

        // Entity management
        Entity CreateEntity()
        {
            EntityID id;

            // Reuse destroyed entity IDs
            if (!m_FreeEntityIDs.empty())
            {
                id = m_FreeEntityIDs.front();
                m_FreeEntityIDs.pop();
            }
            else
            {
                id = m_NextEntityID++;
            }

            return Entity(id, this);
        }

        void DestroyEntity(Entity entity)
        {
            if (!IsValidEntity(entity))
                return;

            EntityID id = entity.GetID();

            // Remove all components for this entity
            for (auto& pair : m_ComponentStorages)
            {
                pair.second->RemoveComponent(id);
            }

            // Mark entity ID as free for reuse
            m_FreeEntityIDs.push(id);
        }

        bool IsValidEntity(Entity entity) const
        {
            return entity.GetID() != NULL_ENTITY && entity.GetID() < m_NextEntityID;
        }

        // Component management
        template<typename T>
        T& AddComponent(Entity entity)
        {
            if (!IsValidEntity(entity))
                throw std::runtime_error("Invalid entity");

            ComponentTypeID typeID = GetComponentTypeID<T>();

            // Get or create component storage
            auto storage = GetOrCreateComponentStorage<T>(typeID);
            return storage->AddComponent(entity.GetID());
        }

        template<typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args)
        {
            if (!IsValidEntity(entity))
                throw std::runtime_error("Invalid entity");

            ComponentTypeID typeID = GetComponentTypeID<T>();

            // Get or create component storage
            auto storage = GetOrCreateComponentStorage<T>(typeID);
            return storage->AddComponent(entity.GetID(), std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent(Entity entity)
        {
            if (!IsValidEntity(entity))
                throw std::runtime_error("Invalid entity");

            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);

            if (!storage)
                throw std::runtime_error("Component type not registered");

            return storage->GetComponent(entity.GetID());
        }

        template<typename T>
        const T& GetComponent(Entity entity) const
        {
            if (!IsValidEntity(entity))
                throw std::runtime_error("Invalid entity");

            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);

            if (!storage)
                throw std::runtime_error("Component type not registered");

            return storage->GetComponent(entity.GetID());
        }

        template<typename T>
        bool HasComponent(Entity entity) const
        {
            if (!IsValidEntity(entity))
                return false;

            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);

            return storage && storage->HasComponent(entity.GetID());
        }

        template<typename T>
        void RemoveComponent(Entity entity)
        {
            if (!IsValidEntity(entity))
                return;

            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);

            if (storage)
                storage->RemoveComponent(entity.GetID());
        }

        // Component iteration
        template<typename T>
        std::vector<Entity> GetEntitiesWith()
        {
            std::vector<Entity> entities;
            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);

            if (storage)
            {
                const auto& entityIDs = storage->GetEntities();
                entities.reserve(entityIDs.size());

                for (EntityID id : entityIDs)
                {
                    entities.emplace_back(id, this);
                }
            }

            return entities;
        }

        // Simple view for single component type
        template<typename T>
        class View
        {
        public:
            View(ComponentStorage<T>* storage, Registry* registry)
                : m_Storage(storage), m_Registry(registry) {
            }

            class Iterator
            {
            public:
                Iterator(size_t index, ComponentStorage<T>* storage, Registry* registry)
                    : m_Index(index), m_Storage(storage), m_Registry(registry) {
                }

                std::pair<Entity, T&> operator*()
                {
                    EntityID entityID = m_Storage->GetEntities()[m_Index];
                    Entity entity(entityID, m_Registry);
                    return { entity, m_Storage->GetComponents()[m_Index] };
                }

                Iterator& operator++() { ++m_Index; return *this; }
                bool operator!=(const Iterator& other) const { return m_Index != other.m_Index; }

            private:
                size_t m_Index;
                ComponentStorage<T>* m_Storage;
                Registry* m_Registry;
            };

            Iterator begin()
            {
                return Iterator(0, m_Storage, m_Registry);
            }

            Iterator end()
            {
                return Iterator(m_Storage ? m_Storage->GetComponentCount() : 0, m_Storage, m_Registry);
            }

        private:
            ComponentStorage<T>* m_Storage;
            Registry* m_Registry;
        };

        template<typename T>
        View<T> GetView()
        {
            ComponentTypeID typeID = GetComponentTypeID<T>();
            auto storage = GetComponentStorage<T>(typeID);
            return View<T>(storage, this);
        }

    private:
        // Get existing component storage
        template<typename T>
        ComponentStorage<T>* GetComponentStorage(ComponentTypeID typeID) const
        {
            auto it = m_ComponentStorages.find(typeID);
            if (it != m_ComponentStorages.end())
            {
                return static_cast<ComponentStorage<T>*>(it->second.get());
            }
            return nullptr;
        }

        // Get or create component storage
        template<typename T>
        ComponentStorage<T>* GetOrCreateComponentStorage(ComponentTypeID typeID)
        {
            auto it = m_ComponentStorages.find(typeID);
            if (it != m_ComponentStorages.end())
            {
                return static_cast<ComponentStorage<T>*>(it->second.get());
            }

            // Create new storage
            auto storage = std::make_unique<ComponentStorage<T>>();
            ComponentStorage<T>* rawPtr = storage.get();
            m_ComponentStorages[typeID] = std::move(storage);

            return rawPtr;
        }

    private:
        EntityID m_NextEntityID;
        std::queue<EntityID> m_FreeEntityIDs;
        std::unordered_map<ComponentTypeID, std::unique_ptr<ComponentStorageBase>> m_ComponentStorages;
    };

    // Implementation of Entity's template methods (here to avoid circular dependency)
    template<typename T>
    T& Entity::AddComponent()
    {
        return m_Registry->AddComponent<T>(*this);
    }

    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return m_Registry->AddComponent<T>(*this, std::forward<Args>(args)...);
    }

    template<typename T>
    T& Entity::GetComponent()
    {
        return m_Registry->GetComponent<T>(*this);
    }

    template<typename T>
    const T& Entity::GetComponent() const
    {
        return m_Registry->GetComponent<T>(*this);
    }

    template<typename T>
    bool Entity::HasComponent() const
    {
        return m_Registry->HasComponent<T>(*this);
    }

    template<typename T>
    void Entity::RemoveComponent()
    {
        m_Registry->RemoveComponent<T>(*this);
    }
}
